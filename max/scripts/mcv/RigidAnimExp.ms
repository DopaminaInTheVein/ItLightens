-----------------------------------------------------------------------------------
-- Auxiliary Methods
-----------------------------------------------------------------------------------

------------------------------------------------------------------
-- Coordinate LH 2 RH Conversion Related Methods
------------------------------------------------------------------

-- Converts a LH transform into a RH one
-- Assumes matrix concatenation by premultiplication
function ConvertLH2RH transform =
(
  -- Build the Z-Up RefSys to Y-Up RefSys conversion matrix
  lh2rh = matrix3 [1,0,0] [0,0,-1] [0,1,0] [0,0,0]
  rh2lh = inverse lh2rh

  -- Convert
  res = rh2lh * transform * lh2rh

  rot = (quat -res.rotation.x -res.rotation.y -res.rotation.z res.rotation.w) as Matrix3
  trans = transMatrix res.translation 
  res2 = rot * trans

  return res2
)

-- Converts a Z-Up point into a Y-Up one
-- Assumes matrix concatenation by premultiplication
function ConvertPointLH2RH point =
(
  -- Build the Z-Up RefSys to Y-Up RefSys conversion matrix
  lh2rh = matrix3 [1,0,0] [0,0,-1] [0,1,0] [0,0,0]

  -- Convert
  res = point * lh2rh

  return res
)

------------------------------------------------------------------
-- Time (Bad-Time, I'd say) Related Methods
------------------------------------------------------------------

-- Converts frame numbers (Xf) to seconds
function FramesToSeconds frames =
(
    ticks = frames as integer
    return ((ticks as float)/4800.0)
)

-- Converts seconds to frame numbers (Xf)
function SecondsToFrames seconds =
(
    return ((seconds * frameRate) as time)
)

-----------------------------------------------------------------------------------
-- <RigidAnimation> XML node export script
-- Main method:
--   ExportAnimation root_node f sample_rate range
-----------------------------------------------------------------------------------

-- Global Settings
global g_AnimationFrameRange
global g_StartTime
global g_EndTime
global g_SampleRate
global g_NumSamples
global g_Duration

-- Aux Functions -------------------------------------------------------------------------

-- Prints an XML Node Transform Animation Track during range with the
-- specified sample rate to f
function PrintXMLChannel node f =
(  
  -- Evaluate and Cache transform keys only once, and then
  -- print transformation pos, rot and scale tracks separatedly.
  track = #()
  for frame = 0 to g_NumSamples-1 do
  (
    -- Compute frame time
    t = g_Duration * (frame / ((g_NumSamples-1) as float)) + g_StartTime
    at time (SecondsToFrames t) transfLH = node.transform

    -- Export Bone transf in Right Handed coordsys (Y-Up)
    append track (ConvertLH2RH transfLH)
  )

  -- Export pos track
  format "<Position>\n" g_NumSamples to: f
    for frame = 1 to g_NumSamples do
    (
      transformRH = track[frame]
      format "% % %\n" transformRH.pos.x transformRH.pos.y transformRH.pos.z to: f
    )
  format "</Position>\n" to: f

  -- Export rot track
  format "<Orientation>\n" g_NumSamples to: f
    for frame = 1 to g_NumSamples do
    (
      transformRH = track[frame]
      format "% % % %\n" transformRH.rotation.x transformRH.rotation.y transformRH.rotation.z transformRH.rotation.w to: f
    )
  format "</Orientation>\n" to: f

)

-- Computes the used animation range for a node
-- \returns (interval starttime endtime)
function ComputeAnimationFrameRange node =
(
    return animationRange
)

--------------------------------------------------------
-- Exports the animation of a single Rigid Object as a <RigidAnimation> XML node
-- \param f = Output File (must be open)
-- \return #(true, TimeRange, Duration, SampleRate, NumFrames) if Success
--         #(false, ErrorString) if Fails
--------------------------------------------------------
function ExportAnimation node f sample_rate range =
(
  -- Compute effective animation frame range
  g_AnimationFrameRange = ComputeAnimationFrameRange( root_node )
  if range.start != -1 do
    if g_AnimationFrameRange.start < range.start do
      g_AnimationFrameRange.start = range.start
  if range.end != -1 do
    if g_AnimationFrameRange.end > range.end do
      g_AnimationFrameRange.end = range.end

  -- Compute start and end time
  g_StartTime = FramesToSeconds g_AnimationFrameRange.start
  g_EndTime = FramesToSeconds g_AnimationFrameRange.end

  -- Compute animation time and frame data
  g_SampleRate = sample_rate
  g_Duration = g_EndTime - g_StartTime
  g_NumSamples = ceil (g_Duration * g_SampleRate) as integer
  -- Using CEIL ensures that the number of frames will be big enough to
  -- guarantee that:
  -- * The final sample rate (g_Duration / g_NumSamples) will be >=
  --   than the user-specified g_SampleRate.
  -- * The first and last max animation frames will be preserved in
  --   the exported animation.

  g_SampleRate = g_NumSamples / g_Duration

  -- Write Rigid Node
  format "<RigidAnimation>\n" to: f

    format "<Name> \"%\" </Name>\n" node.name to: f
    format "<NumSamples> % </NumSamples>\n" g_NumSamples to: f
    format "<Duration> % </Duration>\n" g_Duration to: f

    PrintXMLChannel node f

  format "</RigidAnimation>" to: f

  -- Feedback (bCorrect, TimeRange, Duration, SampleRate, NumFrames)
  return #(true, g_AnimationFrameRange, g_Duration, g_SampleRate, g_NumSamples)
)



----------------------------------------------------------------
-- Rigid Animation Exporter Main Script
----------------------------------------------------------------




-- MAIN --------------------------------------------------------

-- AModal Dialog
-- createDialog RigidAnimExportRollout modal:false width:RigidAnimExportRollout.width height:RigidAnimExportRollout.height