-- forceToFluidsynthRaster -- sets current selection and play cursor
--                            onto fluidsynth raster to be able to compare
--                            live libfluidsynth rendering with external
--                            rendered fluidsynth wave file;
--                            takes project sample rate as the one for
--                            external audio files
--
-- by Dr. TT, 2020

-- --------------------

-- the name of the sample offset variable in the project notes
local _sampleOffsetVariableName = "sampleOffset"

-- --------------------
-- --------------------

function _projectSampleOffset (project)
    -- Returns the sample offset in the notes of <project>

    local result = 0
    local st = reaper.GetSetProjectNotes(project, false, "")
    st = st:gsub("\13\10", "\n")

    -- look for entry with <_sampleOffsetVariableName> and extract
    -- number
    local variablePosition = st:find(_sampleOffsetVariableName, 1, true)
    
    if variablePosition ~= nil then
        local lineEndPosition = st:find("\n", variablePosition, true)
        local equalsPosition  = st:find("=", variablePosition, true)

        if (lineEndPosition ~= nil
            and equalsPosition ~= nil
            and equalsPosition < lineEndPosition) then
            -- extract numeric information
            st = st:sub(equalsPosition + 1, lineEndPosition - 1)
            local number = st:match("%d+")

            if number ~= nil then
                result = tonumber(number)
            end
        end
    end

    return result
end

-- --------------------

function _projectSampleRate (project)
    -- Returns the default sample rate of <project>

    local result = 44100
    local projectSampleRate =
        reaper.GetSetProjectInfo(project, "PROJECT_SRATE", 0, false)

    if projectSampleRate ~= nil then
        result = projectSampleRate
    end

    return result
end
    
-- --------------------

function _rasterizedTime (t, sampleRate, sampleOffset)
    -- Returns nearest time on fluidsynth processing raster for given
    -- time <t> and <sampleRate> and offset given by <sampleOffset>

    local fluidsynthBlockSize = 64
    local blockLength = fluidsynthBlockSize / sampleRate
    local blockCount = math.floor(t / blockLength)
    local adaptedTime = blockCount * blockLength + sampleOffset / sampleRate
    return adaptedTime
end

-- =======================

function main ()
    local project = 0

    local sampleOffset = _projectSampleOffset(project)
    local sampleRate   = _projectSampleRate(project)

    -- adapt play cursor
    local cursorTime = _rasterizedTime(reaper.GetCursorPosition(),
                                       sampleRate, sampleOffset)
    reaper.SetEditCurPos2(project, cursorTime, true, false)

    -- adapt selection boundaries
    local selectionStartTime, selectionEndTime =
        reaper.GetSet_LoopTimeRange2(project, false, true, 0, 0, false)
    selectionStartTime = _rasterizedTime(selectionStartTime,
                                         sampleRate, sampleOffset)
    selectionEndTime   = _rasterizedTime(selectionEndTime,
                                         sampleRate, sampleOffset)
    reaper.GetSet_LoopTimeRange2(project, true, true,
                                 selectionStartTime, selectionEndTime,
                                 false)
end

-- =======================

main()
