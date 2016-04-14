preprocessor_register ("^lua$",
                       function (str) return str:gsub("{STRING}", '{CONTEXT.PRE:match(".-\\"([^\\"]*)\\"")}') end
)

preprocessor_register ("^lua$",
                       function (str) return str:gsub("{FUNCTION}", '+{CONTEXT.PRE:match("function ([^)]*%%))")}+::{DOCVARS.NL} ') end
)

preprocessor_register ("",
                       function (str) return str:gsub("FIXME:([^ ]*) (.*)", 'FIXME:%1 {CONTEXT.FILE}:{CONTEXT.LINE}::{DOCVARS.NL}  %2') end
)

preprocessor_register ("",
                       function (str) return str:gsub("TODO:([^ ]*) (.*)", 'TODO:%1 {CONTEXT.FILE}:{CONTEXT.LINE}::{DOCVARS.NL}  %2') end
)

preprocessor_register ("",
                       function (str) return str:gsub("PLANNED:([^ ]*) (.*)", 'PLANNED:%1 {CONTEXT.FILE}:{CONTEXT.LINE}::{DOCVARS.NL}  %2') end
)

--PLANNED: noweb like preprocessing syntax for chapter substitutions in textfiles
