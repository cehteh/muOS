preprocessor_register ("^lua$",
                       function (str) return str:gsub("{STRING}", '{CONTEXT.PRE:match(".-\\"([^\\"]*)\\"")}') end
)

preprocessor_register ("^lua$",
                       function (str) return str:gsub("{FUNCTION}", '+{CONTEXT.PRE:match("function ([^)]*%%))")}+::{DOCVARS.NL} ') end
)


preprocessor_register ("^c$",
                       function (str)
                         return str:gsub("{ERRORDEF}(.-) ?%*/.*",
                                         '+muos_{CONTEXT.PRE:match("MUOS_ERROR%%((.-)%%)")}+::{DOCVARS.NL} %1')
                       end
)


function git_blame (file, line)
  local result = {}
  local gitcmd = "git blame '"..file.."' -M -L "..tostring(line)..",+1 -p 2>/dev/null"
  dbg ("git:", gitcmd)
  local git = io.popen(gitcmd)

  for line in git:lines() do
    local k,v = line:match("^([%a-]+) (.*)")
    if line:match("^([%w]+) (%d+) ") then
      k = 'revision'
      v = line:match("^(%w*)")
    elseif line:match("^\t") then
      k = 'line'
      v = line:match("^\t(.*)")
    end
    result[k] = v
  end
  local _,_,exitcode = git:close()
  return exitcode == 0 and result or nil
end

function git_blame_context ()
  local blame = git_blame (CONTEXT.FILE, CONTEXT.LINE)
  if blame then
    return " +"..DOCVARS.NL..
      "  _"..blame.summary.."_ +"..DOCVARS.NL..
      "  "..blame.author.." "..os.date("%c", tonumber(blame["author-time"])).." +"..DOCVARS.NL..
      "  +"..tostring(blame.revision).."+"
  else
    return ""
  end
end

preprocessor_register ("",
                       function (str) return str:gsub("FIXME:([^ ]*) (.*)", 'FIXME:%1 {CONTEXT.FILE}:{CONTEXT.LINE}::{DOCVARS.NL}  %2{git_blame_context ()}{DOCVARS.NL}') end
)

preprocessor_register ("",
                       function (str) return str:gsub("TODO:([^ ]*) (.*)", 'TODO:%1 {CONTEXT.FILE}:{CONTEXT.LINE}::{DOCVARS.NL}  %2{git_blame_context ()}{DOCVARS.NL}') end
)

preprocessor_register ("",
                       function (str) return str:gsub("PLANNED:([^ ]*) (.*)", 'PLANNED:%1 {CONTEXT.FILE}:{CONTEXT.LINE}::{DOCVARS.NL}  %2{git_blame_context ()}{DOCVARS.NL}') end
)

--PLANNED: noweb like preprocessing syntax for chapter substitutions in textfiles
