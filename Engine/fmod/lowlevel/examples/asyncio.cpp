/*===============================================================================================
 Async IO Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2016.

 This example shows how to play a stream and use a custom file handler that defers reads for the
 streaming part.  FMOD will allow the user to return straight away from a file read request and
 supply the data at a later time.
===============================================================================================*/
#define _CRT_SECURE_NO_WARNINGS

#include <process.h> 

#include "fmod.hpp"
#include "common.h"

Common_Mutex gCrit;

typedef struct dataqueue
{
    struct dataqueue *next;
    FMOD_ASYNCREADINFO *info;
    bool busy;
} dataqueue;

dataqueue queuehead = { &queuehead, 0, false };
bool gThreadQuit = false;
bool gThreadFinished = false;
bool gSleepBreak = false;

/*
    A little text buffer to allow a scrolling window
*/
char line[NUM_ROWS][NUM_COLUMNS];
void AddLine(char *s)
{
    int count;
    for (count = 1; count < NUM_ROWS; count++)
    {
        memcpy(line[count-1], line[count], NUM_COLUMNS);
    }
    strncpy(line[NUM_ROWS-1], s, NUM_COLUMNS);
}

/*
    File callbacks
*/
FMOD_RESULT F_CALLBACK myopen(const char *name, unsigned int *filesize, void **handle, void * /*userdata*/)
{
    if (name)
    {
        FILE *fp;

        fp = fopen(name, "rb");
        if (!fp)
        {
            return FMOD_ERR_FILE_NOTFOUND;
        }

        fseek(fp, 0, SEEK_END);
        *filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *handle = fp;
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myclose(void *handle, void * /*userdata*/)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    fclose((FILE *)handle);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myread(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void * /*userdata*/)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (bytesread)
    {
        *bytesread = (int)fread(buffer, 1, sizebytes, (FILE *)handle);   
        if (*bytesread < sizebytes)
        {
            return FMOD_ERR_FILE_EOF;
        }
    }


    return FMOD_OK;
}


FMOD_RESULT F_CALLBACK myseek(void *handle, unsigned int pos, void * /*userdata*/)
{
    char s[NUM_COLUMNS]; Common_snprintf(s, NUM_COLUMNS, "************ seek to %d\n", pos); AddLine(s);
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
   
    fseek((FILE *)handle, pos, SEEK_SET);

    return FMOD_OK;
}


FMOD_RESULT F_CALLBACK myasyncread(FMOD_ASYNCREADINFO *info, void * /*userdata*/)
{
    dataqueue *current;

    char s[NUM_COLUMNS]; Common_snprintf(s, NUM_COLUMNS, "REQUEST %5d bytes, offset %5d PRIORITY = %d.\n", info->sizebytes, info->offset, info->priority); AddLine(s);
    
    current = (dataqueue *)calloc(sizeof(dataqueue), 1);
    if (!current)
    {
        info->done(info, FMOD_ERR_MEMORY);
        return FMOD_ERR_MEMORY;
    }
    
    Common_Mutex_Enter(&gCrit);

    current->next = queuehead.next;    
    current->info = info;
    queuehead.next = current;
    
    if (info->priority > 50)
    {
        gSleepBreak = true;     /* Tell file thread to stop sleeping, we have an urgent request here! */
    }
    
    Common_Mutex_Leave(&gCrit);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK myasynccancel(FMOD_ASYNCREADINFO *info, void * /*userdata*/)
{
    dataqueue *current, *last;
    
    Common_Mutex_Enter(&gCrit);

    last    = &queuehead;
    current = last->next;

    while (current != &queuehead)
    {    
        dataqueue *next = current->next;

        if (current->info->handle == info->handle && !current->busy)
        {
            last->next = next;
            current->info->done(current->info, FMOD_ERR_FILE_DISKEJECTED);
            free(current);
        }
        else
        {
            last = current;
        }

        current = next;
    }

    Common_Mutex_Leave(&gCrit);

    return FMOD_OK;
}


unsigned int __stdcall ProcessQueue(void * /*param*/)
{
    while (!gThreadQuit)
    {
        int count;
        dataqueue *current;
        
        Common_Mutex_Enter(&gCrit);

        current = queuehead.next;
        while (current != &queuehead)
        {    
            unsigned int toread;
            
            current->busy = true;
            
            Common_Mutex_Leave(&gCrit);  

            toread = current->info->sizebytes;
            if (toread > 16384)
            {
                toread = 16384;     /* For fun - Let's deprive the read of the whole block.  Only give 16kb at a time to make it re-ask for more later. */
            }           

            for (count = 0; count < 50; count++)
            {
                Sleep(10);
                if (gSleepBreak)
                {
                    AddLine("URGENT REQUEST - reading now!\n");
                    gSleepBreak = false;
                    break;
                }
            }
            
            fseek((FILE *)current->info->handle, current->info->offset, SEEK_SET);           
            current->info->bytesread = fread(current->info->buffer, 1, toread, (FILE *)current->info->handle);
            
            if (current->info->bytesread < toread)
            {
                char s[NUM_COLUMNS]; Common_snprintf(s, NUM_COLUMNS, "FED     %5d bytes, offset %5d (* EOF)\n", current->info->bytesread, current->info->offset); AddLine(s);
                current->info->done(current->info, FMOD_ERR_FILE_EOF);
            }
            else
            {
                char s[NUM_COLUMNS]; Common_snprintf(s, NUM_COLUMNS, "FED     %5d bytes, offset %5d\n", current->info->bytesread, current->info->offset); AddLine(s);
                current->info->done(current->info, FMOD_OK);
            }
            
            Common_Mutex_Enter(&gCrit);
            
            current->busy = false;
            queuehead.next = current->next;
            free(current);
            current = queuehead.next;
        }

        Common_Mutex_Leave(&gCrit);       
    }
    
    gThreadFinished = true;

    _endthreadex(0);
   
    return 0;
}



int FMOD_Main()
{
    FMOD::System     *system;
    FMOD::Sound      *sound;
    FMOD::Channel    *channel = 0;
    FMOD_RESULT       result;
    unsigned int      version;
    HANDLE            threadhandle;
    void            *extradriverdata = 0;

    Common_Init(&extradriverdata);

    Common_Mutex_Create(&gCrit);
    
	threadhandle = (HANDLE)_beginthreadex(NULL, 0, ProcessQueue, 0, 0, 0);
    if (!threadhandle)
    {
        printf("Failed to create file thread.\n");
        return 0;
    }
    
    /*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }

    result = system->init(1, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    result = system->setStreamBufferSize(32768, FMOD_TIMEUNIT_RAWBYTES);
    ERRCHECK(result);
    
    result = system->setFileSystem(myopen, myclose, myread, myseek, myasyncread, myasynccancel, 2048);
    ERRCHECK(result);

    result = system->createStream(Common_MediaPath("wave.mp3"), FMOD_LOOP_NORMAL | FMOD_2D | FMOD_IGNORETAGS, 0, &sound);
    ERRCHECK(result);

    result = system->playSound(sound, 0, false, &channel);
    ERRCHECK(result);

    /*
        Main loop.
    */
    do
    {
        Common_Update();

        if (sound)
        {
            FMOD_OPENSTATE openstate;
            bool starving;

            sound->getOpenState(&openstate, 0, &starving, 0);
            
            if (starving)
            {
                AddLine("Starving\n");
                result = channel->setMute(true);
            }
            else
            {
                result = channel->setMute(false);
                ERRCHECK(result);
            }
        }

        if (Common_BtnPress(BTN_ACTION1))
        {
            result = sound->release();
            if (result == FMOD_OK)
            {
                sound = 0;
                AddLine("Released sound.\n");
            }
            break;
        }

        result = system->update();
        ERRCHECK(result);

        Common_Draw("==================================================");
        Common_Draw("Stream IO Example.\n");
        Common_Draw("Copyright (c) Firelight Technologies 2004-2016.");
        Common_Draw("==================================================");
        Common_Draw("");
        Common_Draw("Press %s to release playing stream", Common_BtnStr(BTN_ACTION1));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
        Common_Draw("");
        
        for (int count = 0; count < NUM_ROWS; count++)
        {
            Common_Draw(line[count]);
        }

        Common_Sleep(10);

    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    if (sound)
    {
        result = sound->release();
        ERRCHECK(result);
    }
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    gThreadQuit = true;
    while (!gThreadFinished)
    {
        Common_Sleep(10);
    }

    Common_Mutex_Destroy(&gCrit);
    Common_Close();

    return 0;
}
