--license:
--: pipadoc - Documentation extractor
--: Copyright (C)                        Pipapo Project
--:  2015,                               Christian Thaeter <ct@pipapo.org>
--:
--: This program is free software: you can redistribute it and/or modify
--: it under the terms of the GNU General Public License as published by
--: the Free Software Foundation, either version 3 of the License, or
--: (at your option) any later version.
--:
--: This program is distributed in the hope that it will be useful,
--: but WITHOUT ANY WARRANTY; without even the implied warranty of
--: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--: GNU General Public License for more details.
--:
--: You should have received a copy of the GNU General Public License
--: along with this program.  If not, see <http://www.gnu.org/licenses/>.

--TODO: include operator, add a file to the processing list
--FIXME: split on newlines

local LINE = 0
local FILE = "<startup>"

local docvars = {
  --docvars:nl `NL`::
  --docvars:nl   The linebreak character sequence, usually '\n' on unix systems but
  --docvars:nl   can be changed with a commandline option
  --FIXME: define in config? needs escapes
  NL = "\n",

  -- defaults
  MARKUP = "plain",
}

local args_done = false
local opt_verbose = 1
local opt_nodefaults = false
local opt_toplevel = "MAIN"
local opt_inputs = {false}
local opt_config = "pipadoc.pconf"


--PLANNED: log to PIPADOC_LOG section, later hooked in here
local printerr_hook

function printerr(...)
  local line = ""

  for i,v in ipairs {...} do
    if i > 1 then
      line = line.."\t"
    end
    line = line..tostring(v)
  end
  line = line.."\n"

  io.stderr:write(line)
  if printerr_hook then
    printerr_hook(line)
  end
end

function msg(lvl,...)
  if lvl <= opt_verbose then
    printerr(FILE..":"..(LINE ~= 0 and LINE..":" or ""), ...)
  end
end


--api:
--:
--: Logging Progress and Errors
--: ~~~~~~~~~~~~~~~~~~~~~~~~~~~
--:
--: There are few function to log progress and report errors. All this functions take a
--: variable argument list. Any Argument passed to them will be converted to a string and printed
--: to stderr when the verbosity level is high enough.
--:
function warn(...) msg(1, ...) end  --: {$$$FUNCTION}::{$NL} report a important but non fatal failure
function echo(...) msg(2, ...) end  --: {$$$FUNCTION}::{$NL} report normal progress
function dbg(...) msg(3, ...) end   --: {$$$FUNCTION}::{$NL} show debugging information
function trace(...) msg(4, ...) end --: {$$$FUNCTION}::{$NL} show more detailed progress information


--PLANNED: use echo() for progress

function die(...) --: {$$$FUNCTION}::{$NL} report a fatal error and exit the programm
  printerr(...)
  os.exit(1)
end



-- debugging only
function dump_table(p,t)
  for k,v in pairs(t) do
    dbg(p,k,v)
    if type(v) == 'table' then
      dump_table(p.."/"..k, v)
    end
  end
end


--api:
--:
--: Optional Lua Modules
--: ~~~~~~~~~~~~~~~~~~~~
--:
--: 'pipadoc' does not depend on any nonstandard Lua libraries, because they may not be installed on the target
--: system. But some modules can be loaded optionally to augment its behavior and provide extra features.
--: Plugin-writers should try to adhere to this practice if possible and use the 'request' function instead the
--: Lua 'require'.
--:
--: When luarocks is installed, then the 'luarocks.loader' is loaded by default to make any module installed by
--: luarocks available.
--:
function request(name) --: {$$$FUNCTION}::{$NL} try to load optional modules
  --:    wraps lua 'require' in a pcall so that failure to load a module results in 'nil' rather than a error
  local ok,handle = pcall(require, name)
  if ok then
    if handle._VERSION then
      dbg("loaded:", name, handle._VERSION)
    end
    return handle
  else
    warn("Can't load module:", name)
    return nil
  end
end

--TODO: docme
-- fake an interface
local function fake(metatable, tab)
  tab = tab or {}
  tab.FAKE = true
  return setmetatable(tab, metatable or {__index = function () end})
end

request "luarocks.loader"
--PLANNED: for pattern matching etc
--lfs = request "lfs"
--posix = request "posix"

local strsubst

local function load_modules()
  strsubst = request "strsubst" or fake
  {
    __index = function (tab, key)
      return tab._index[key]
    end,

    __newindex = function (tab, key, value)
      tab._index[key] = value
    end,

    __call = function(tab, op, ...)
      if type(op) == 'table' then
        rawset(tab, "__index", op)
      end
      return op
    end
  }

  strsubst (docvars)
end


--api:
--:
--: Typechecks
--: ~~~~~~~~~~
--:
--: Some wrapers around 'assert' to check externally supplied data. On success 'var' will be returned
--: otherwise an assertion error is raised.
--:
function assert_type(var, expected) --: {$$$FUNCTION}::{$NL} checks that the 'var' is of 'type'
  assert(type(var) == expected, "type error: "..expected.." expected, got "..type(var))
  return var
end

function maybe_type(var, expected) --: {$$$FUNCTION}::{$NL} checks that the 'var' is of 'type' or nil
  assert(var == nil or type(var) == expected, "type error: "..expected.." or nil expected, got "..type(var))
  return var
end

function assert_char(var) --: {$$$FUNCTION}::{$NL} checks that 'var' is a single character
  assert(type(var) == "string" and #var == 1, "type error: single character expected")
  return var
end

function assert_notnil(var) --: {$$$FUNCTION}::{$NL} checks that 'var' is not 'nil'
  assert(type(var) ~= "nil", "Value expected")
  return var
end


function to_table(v) --: {$$$FUNCTION}::{$NL} if 'v' is not a table then return {v}
  if type(v) ~= 'table' then
    v = {v}
  end
  return v
end

--sections:
--: Text in pipadoc is appended to named 'sections'. Sections are later brought into the desired order in a 'toplevel'
--: section. Additionally instead just appending text to a named section, text can be appended under some
--: alphanumeric key under that section. This enables later sorting for indexes and glossaries.
--:
--: Sections can span a block of lines (until another section is set) or one-line setting a section only for the current
--: comment line. Blocks are selected when the pipadoc comment defines a section and maybe a key but the rest of the
--: is empty, every subsequent line which does not define a new section block is then appended to the current block.
--:
--: One-Line sections are selected when a section and maybe a key are followed by some documentation text.
--:
--: When no section is set in a file, then the block section name defaults to the files name up, but excluding to the first dot.
--:
--: Pipadoc needs a receipe how to assemble all sections togehter. This is done in a 'toplevel' section which defaults to the
--: name 'MAIN'.
--:
--: .An example document (assume the name example.sh)
--: ----
--: #!/bin/sh
--: #: here the default section is 'example', derrived from 'example.sh'
--: #oneline:o this is appended to the section 'oneline' under key 'o'
--: #: back to the 'example' section
--: #newname:
--: #: this starts a new section block named 'newname'
--: #oneline:a this is appended to the section 'oneline' under key 'a'
--: #MAIN:
--: #: Assemble the document
--: #: first the 'newname'
--: #=newname
--: #: then 'example'
--: #=example
--: #: and finally 'oneline' alphabetically sorted by keys
--: #@oneline
--: ----
--:
--: Will result in
--: ----
--: Assemble the document
--: first the 'newname'
--: this starts a new section block named 'newname'
--: then 'example'
--: here the default section is 'example', derrived from 'example.sh'
--: back to the 'example' section
--: and finally 'oneline' alphabetically sorted by keys
--: this is appended to the section 'oneline' under key 'a'
--: this is appended to the section 'oneline' under key 'o'
--: ----
local sections = {}
local sections_usecnt = {}
local sections_keys_usecnt = {}

--api:
--:
--: Sections
--: ~~~~~~~~
--:

--PLANNED: maybe append the context       , docvars.SECTION, docvars.ARG, docvars.OP,  docvars.TEXT, docvars.PRE

function section_append(section, key, context) --: {$$$FUNCTION}::
  --:   section:::
  --:     name of the section to append to, must be a string
  assert_type(section, "string")
  --:   key:::
  --:     the subkey for sorting within that section. 'nil' for appending text to normal sections
  maybe_type(key, "string")
  --:   context:::
  --FIXME: DOCME    the parameter for the action, usually the text to be included in the output.
  maybe_type(context, "table")
  --:
  --:   Append data to the given section/key
  trace("append:", section, key, context)
  sections[section] = sections[section] or {keys = {}}
  if key and #key > 0 then
    sections[section].keys[key] = sections[section].keys[key] or {}
    table.insert(sections[section].keys[key], context)
  else
    table.insert(sections[section], context)
  end
--FIXME:  trace(action.. ":", section.."["..(key and #key > 0 and key .."]["..#sections[section].keys[key] or #sections[section]).."]:", context.TEXT)
end

--api:
function section_get(section, key, index) --: {$$$FUNCTION}::
  --:   section:::
  --:     name of the section to append to, must be a string
  assert_type(section, "string")
  --:   key:::
  --:     the subkey for sorting within that section. maybe 'nil'
  --:   index:::
  --:     line number to query, when 'nil' defaults to the last line
  --:   returns:::
  --:     value or nil
  --:
  --:   query the value of the given section/key at index (or at end)
  if not sections[section] then
    return
  end

  if not key then
    index = index or #sections[section]
--FIXME:    echo("nokey", index, sections[section][index].action, sections[section][index].text)
    return sections[section][index]
  else
    index = index or #sections[section].keys
    echo("key", sections[section].keys[key][index])
    return sections[section].keys[key][index]
  end
end


--filetypes:
--: Pipadoc needs to know about the syntax of line comments of the files it is reading. For this one can
--: register patterns to be matched against the filename together with a list of line comment characters.
--:
--: Pipadoc includes definitions for some common filetypes already. For languages which support block comments
--: the opening (but not the closing) commenting characters are registered as well. This allows for example to
--: define section blocks right away. But using the comment closing sequence right on the line will clobber the
--: output, just don't do that!
--:
--: .Example in C
--: ----
--: /*blocksection:
--: //: this is a block-section
--: //: line comment sequences inside the block comment are still required
--: */
--:
--: /*works_too: but looks ugly
--: */
--:
--: /*fail: don't do this, pipadoc comments span to the end of the line */ void* text;
--: ----
--:
--: A special case is that when a line comment is defined as an empty string ("") then every line of a file is
--: considered as documentation but no special operations apply. This is used for parsing plaintext documentation
--: files. Which also uses the "PIPADOC:" keyword to enable special operations within text files.
local filetypes = {}


--api:
--:
--: Filetypes
--: ~~~~~~~~~
--:
function filetype_register(name, filep, linecommentseqs) --: {$$$FUNCTION}::
  --:     name:::
  --:       name of the language
  --:     filep:::
  --:       a Lua pattern or list of patterns matching filename
  --:     linecommentseqs:::
  --:       a string or list of strings matching comments of the registered filetype
  --:
  --: Register a new filetype.
  --:
  --: For example, C and C++ Filetypes are registered like:
  --:
  --:  filetype_register("C", {"%.c$","%.cpp$","%.C$", "%.cxx$", "%.h$"}, {"//", "/*"})
  --:
  assert_type(name, "string")

  filep = to_table(filep)
  for _,v in pairs(filep) do
    assert_type(v, "string")
  end

  linecommentseqs = to_table(linecommentseqs)
  for _,v in pairs(linecommentseqs) do
    assert_type(v, "string")
  end

  for i=1,#filep do
    filetypes[filep[i]] = filetypes[filep[i]] or {language = name}
    for j=1,#linecommentseqs do
      dbg("register filetype:", name, filep[i], linecommentseqs[j])
      filetypes[filep[i]][#filetypes[filep[i]]+1] = linecommentseqs[j]
    end
  end
end

function filetype_get(filename)
  assert_type(filename, "string")
  for k,v in pairs(filetypes) do
    if filename:match(k) then
      return v,k
    end
  end
end

function filetype_select(line, linecommentseqs)
  for i=1,#linecommentseqs do
    if string.match(line, linecommentseqs[i]) then
      return linecommentseqs[i]
    end
  end
end


--op:
--: Operators define the core functionality of pipadoc. They are mandatory in the pipadoc syntax
--: to define a pipadoc comment line. It is possible (but rarely needed) to define additional
--: operators. Operators must be a single punctuation character
local operators = {}


--TODO: operator_register(char, read, generate) .. add generator function here too
--api:
function operator_register(char, func) --: {$$$FUNCTION}::
  --:   char:::
  --:     single punctuation character defining this operator
  --:   func:::
  --:     a function which receives a table of the 'context' parsed from the pipadoc commentline
  --:
  --: Operators drive the main functionality, like invoking the processors and generating the output.
  --:
  assert(string.match(char, "^%p$") == char)
  dbg("register operator:", char)
  operators[char] = assert_type(func, 'function')
end

function operator_pattern()
  local pattern=""
  for k in pairs(operators) do
    pattern = pattern..k
  end
  return "["..pattern.."]"
end


--usage:
local options = {
  "pipadoc [options...] [inputs..]",  --:  {$$$STRING}
  "  options are:", --:  {$$$STRING}

  "    -v, --verbose                           increment verbosity level", --:  {$$$STRING}
  ["-v"] = "--verbose",
  ["--verbose"] = function () opt_verbose = opt_verbose+1 end,

  "    -q, --quiet                             supresses any messages", --:  {$$$STRING}
  ["-q"] = "--quiet",
  ["--quiet"] = function () opt_verbose = 0 end,

  "    -d, --debug                             set verbosity to maximum", --:  {$$$STRING}
  ["-d"] = "--debug",
  ["--debug"] = function () opt_verbose = 3 end,

  "    -h, --help                              show this help", --:  {$$$STRING}
  ["-h"] = "--help",
  ["--help"] = function ()
    print("usage:")
    for i=1,#options do
      print(options[i])
    end
    os.exit(0)
  end,


  "    -r, --register <name> <file> <comment>  register a filetype pattern", --:  {$$$STRING}
  "                                            for files matching a file pattern", --:  {$$$STRING}
  ["-r"] = "--register",
  ["--register"] = function (arg,i)
    assert(type(arg[i+3]))
    filetype_register(arg[i+1], arg[i+2], arg[i+3])
    return 2
  end,


  "    -t, --toplevel <name>                   sets 'name' as toplevel node [MAIN]", --:  {$$$STRING}
  ["-t"] = "--toplevel",
  ["--toplevel"] = function (arg, i)
    assert(type(arg[i+1]))
    opt_toplevel = arg[i+1]
    return 1
  end,

  "    -c, --config <name>                     selects a configfile [pipadoc.conf]", --:  {$$$STRING}
  ["-c"] = "--config",
  ["--config"] = function (arg, i)
    assert(type(arg[i+1]))
    opt_config = arg[i+1]
    return 1
  end,


  "    --no-defaults                           disables default filetypes and processors", --:  {$$$STRING}
  ["--no-defaults"] = function () opt_nodefaults = true end,


  "    -m, --markup <name>                     selects the markup engine for the output [plain]", --:  {$$$STRING}
  ["-m"] = "--markup",
  ["--markup"] = function (arg, i)
    assert(type(arg[i+1]))
    docvars.MARKUP = arg[i+1]
    return 1
  end,

  "    --                                      stops parsing the options and treats each", --:  {$$$STRING}
  "                                            following argument as input file", --:  {$$$STRING}
  ["--"] = function () args_done=true end,

  --TODO: --alias match pattern --file-as match filename
  --TODO: -o --output
  --TODO: -l --load
  --TODO: --features  show a report which features (using optional lua modules) are available
  --TODO: list-operators
  --TODO: list-sections
  --TODO: force filetype variant  foo.lua:.txt
  --TODO: wordwrap
  --TODO: eat empty lines
  --TODO: add debug report (warnings/errors) to generated document PIPADOC_LOG section
  --TODO: lineending \n \r\n
  --TODO: wrap at blank/intelligent
  --PLANNED: wordwrap

  "", --:  {$$$STRING}
  "  inputs are filenames or a '-' which indicates standard input", --:  {$$$STRING}
}

function parse_args(arg)
  local i = 1
  while i <= #arg do
    while string.match(arg[i], "^%-%a%a+") do
      parse_args {"-"..string.sub(arg[i],2,2)}
      arg[i] = "-"..string.sub(arg[i],3)
    end

    if not options[arg[i]] then
      opt_inputs[#opt_inputs+1] = arg[i]
    else
      local f = options[arg[i]]
      while options[f] do
        f = options[f]
      end
      if type(f) == 'function' then
        i = i + (f(arg, i) or 0)
      else
        die("optarg error")
      end
    end
    i = i+1
  end
end

function setup()
  parse_args(arg)
  load_modules()

  do
    local date = os.date ("*t")
    --docvars:date `YEAR, MONTH, DAY, HOUR, MINUTE`::
    --docvars:date   Current date information
    docvars.YEAR = date.year
    docvars.MONTH = date.month
    docvars.DAY = date.day
    docvars.HOUR = date.hour
    docvars.MINUTE = date.min
    --docvars:date `DATE`::
    --docvars:date   Current date in YEAR/MONTH/DAY format
    docvars.DATE = date.year.."/"..date.month.."/"..date.day
  end

  if not opt_nodefaults then
    --PLANNED: read style file like a config, lower priority, differnt paths (./ /etc/ ~/ ...)
    if not strsubst.FAKE then
      local config = io.open(opt_config)
      if config then
        dbg("load config:", opt_config)
        strsubst(config:read("*a"))
        config:close()
      else
        warn("failed config:", opt_config)
      end
    end

    --filetypes_builtin:scons * SCons
    filetype_register("scons", "^SConstuct$", "#")

    --filetypes_builtin:cmake * CMake
    filetype_register("cmake", {"^CMakeLists.txt$","%.cmake$"}, {"#", "#[["})

    --filetypes_builtin:c * C, C++, Headerfiles
    filetype_register("c", {"%.c$","%.cpp$", "%.C$", "%.cxx$", "%.h$", "%.hpp$", "%.hxx$"}, {"//", "/*"})

    --filetypes_builtin:lua * Lua
    filetype_register("lua", {"%.lua$"}, "%-%-")

    --filetypes_builtin:automake * Autoconf, Automake
    filetype_register("automake", {"%.am$", "%.in$", "^configure.ac$"}, {"#", "dnl"})

    --filetypes_builtin:make * Makefiles
    filetype_register("makefile", {"^Makefile$", "%.mk$", "%.make$"}, "#")

    --filetypes_builtin:shell * Shell, Perl, AWK
    filetype_register("shell", {"%.sh$", "%.pl$", "%.awk$", }, "#")

    --filetypes_builtin:prolog * Prolog
    filetype_register("prolog", {"%.pro$", "%.P$"}, "%")

    --filetypes_builtin:text * Textfiles, Pipadoc (`.pdoc`)
    filetype_register("plain", {"%.txt$", "%.TXT$", "%.pdoc$", "^-$"}, {"PIPADOC:", ""})

    --filetypes_builtin:pipadocconfig * Pipadoc config (`.pconf`)
    filetype_register("pconf", "%.pconf$", {"#", "^ "})

    --filetypes_builtin:java * Java, C#
    filetype_register("java", {"%.java$", "%.cs$"}, {"//", "/*"})

    --filetypes_builtin:objective_c * Objective-C
    filetype_register("objc", {"%.h$", "%.m$", "%.mm$"}, {"//", "/*"})

    --filetypes_builtin:python * Python
    filetype_register("python", "%.py$", "#")

    --filetypes_builtin:visualbasic * Visual Basic
    filetype_register("visualbasic", "%.vb$", "'")

    --filetypes_builtin:php * PHP
    filetype_register("php", "%.php%d?$", {"#", "//", "/*"})

    --filetypes_builtin:javascript * Javascript
    filetype_register("javascript", "%.js$", "//", "/*")

    --filetypes_builtin:delphi * Delphi, Pascal
    filetype_register("delphi", {"%.p$", "%.pp$", "^%.pas$"}, {"//", "{", "(*"})

    --filetypes_builtin:ruby * Ruby
    filetype_register("ruby", "%.rb$", "#")

    --filetypes_builtin:sql * SQL
    filetype_register("sql", {"%.sql$", "%.SQL$"}, {"#", "--", "/*"})
  end

  --op_builtin:
  --: `:` ::
  --:   The documentation operator. Defines normal documentation text. Each pipadoc comment using the `:`
  --:   operator is processed as potential documentation. First all enabled 'processors' are run over it and
  --:   finally the text is appended to the current section(/key)
  operator_register(
    ":",
    function (context)
      section_append(context.SECTION, context.ARG, context)
    end
  )


  --op_builtin:
  --: `=` ::
  --:   Section paste operator. Takes a section name as argument and will paste that section in place.
  operator_register(
    "=",
    function (context)
      --PLANNED: how to use docvars.text?
      if #context.ARG > 0 then
        section_append(context.SECTION, nil, context)
      else
        warn("include argument missing:")
      end
    end
  )

  --op_builtin:
  --: `@` ::
  --:   Takes a section name as argument and will paste section text alphabetically sorted by their keys.
  --PLANNED: option for sorting locale
  --PLANNED: option for sorting (up/dowen)
  operator_register(
    "@",
    function (context)
      if #context.ARG > 0 then
        section_append(context.SECTION, nil, context)
      else
        warn("sort section missing:")
      end
    end
  )


  --op_builtin:
  --: `#` ::
  --:   Takes a section name as argument and will paste section text numerically sorted by their keys.
  --PLANNED: option for sorting (up/down)
  operator_register(
    "#",
    function (context)
      if #context.ARG > 0 then
        section_append(context.SECTION, nil, context)
      else
        warn("sort section missing:")
      end
    end
  )
end

local dbg_section
local SECTION
local ARG

function process_line (line, comment)
  local context = {}

  -- special case for plaintext files
  if comment == "" then
    context.PRE, context.COMMENT, context.SECTION, context.OP, context.ARG, context.TEXT =
      "", " ", "", ":", "", line
  else
    context.PRE, context.COMMENT, context.SECTION, context.OP, context.ARG, context.TEXT =
      --FIXME: create opchars dynamically
      string.match(line,"^(.-)("..comment..")([%w_.]*)([:=@#])([%w_.]*)%s?(.*)$")
  end

  context.LANGUAGE = docvars.LANGUAGE

  --FIXME: wrong section
  --docvars:file `FILE`::
  --docvars:file   The file or section name currently processed or some special annotation
  --docvars:file   in angle brakets (eg '<startup>') on other processing phases
  --docvars:line `LINE`::
  --docvars:line   Current line number of input or section, or indexing key
  --docvars:line   Lines start at 1, if set to 0 then some output formatters skip over it
  context.FILE, context.LINE = FILE, LINE

  warn("parsed section:", context.SECTION, SECTION)

  
  if context.PRE then
    if context.SECTION == "" then
      context.SECTION = SECTION
      -- context.ARG = ARG
      trace("pre:", context.PRE, "section:", context.SECTION, "op:", context.OP, "arg:", context.ARG, "text:", context.TEXT)

    else
      if dbg_section ~= context.SECTION then
        dbg("pre:", context.PRE, "section:", context.SECTION, "op:", context.OP, "arg:", context.ARG, "text:", context.TEXT)
        dbg_section = context.SECTION
      end

      if context.TEXT == "" then
        SECTION = context.SECTION
        -- ARG = context.ARG
        return
      end
    end

    local op = context.OP
    if op then
      if operators[op] then
        while operators[op] do
          op = operators[op]
        end
        op(context)
      else
        warn("unknown operator:", op)
      end
    end
  end
end

function process_file(file)
  --FIXME: first check if file is available, then warn
  local descriptor, pattern = filetype_get(file)
  if not descriptor then
    warn("unknown file type:", file)
    return
  end

  ARG = ""

  local fh
  if file == '-' then
    FILE = "<stdin>"
    fh = io.stdin
  else
    fh = io.open(file)
    if not fh then
      warn("file not found:", file)
      return
    end
    FILE = file
  end

  --FIXME: wrong docsection
  --docvars:section `SECTION`::
  --docvars:section   stores the current section name
  SECTION = FILE:match("[^./]+%f[.\0]")
  LINE = 0
  dbg("section:", SECTION)

  docvars.LANGUAGE = descriptor.language
  dbg("LANGUAGE:", docvars.LANGUAGE)

  for line in fh:lines() do
    LINE = LINE+1
    trace("input:", line)
    local comment = filetype_select(line, descriptor)
    if comment then
      process_line(line, comment)
    end
  end
  fh:close()
end

function process_inputs()
  for i=1,#opt_inputs do
    if opt_inputs[i] ~= false then
      --TODO: globbing if no such file exists
      process_file(opt_inputs[i])
    end
  end
end




local default_generators = {
  [":"] = function (context)
    local ret = strsubst(context.TEXT, context)
    if ret == "" and context.TEXT ~= "" then
      return ""
    else
      trace ("generate:", ret)
      return ret.."\n"
    end
  end,

  ["="] = function (context)
    return generate_output(context.ARG)
  end,

  ["@"] = function (context)
    dbg("generate_output_sorted:", order, which, opt)
    local which = context.ARG
    local section = sections[context.ARG].keys
    local text = ""

    if section ~= nil then
      sections_keys_usecnt[which] = sections_keys_usecnt[which] + 1

      local oldfile = FILE
      FILE ='<output>:'..which

      local sorted = {}

      for k in pairs(section) do
        table.insert(sorted, k)
      end

      table.sort(sorted, function(a,b) return tostring(a) < tostring(b) end)

      if #sorted == 0 then
        warn("section is empty:",which)
        return ""
      end

      for i=1,#sorted do
        LINE=sorted[i]
        for j=1,#section[sorted[i]] do
          text = text..section[sorted[i]][j].TEXT..'\n'
        end
      end
      FILE = oldfile
    else
      warn("no section named:", which)
    end
    return text
  end,

  ["#"] = nil,
}




local sofar_rec={}

function generate_output(which, generators)
  dbg("generate_output:", which)
  generators = generators or default_generators
  local section = sections[which]
  local text = ""

  if section ~= nil then
    if sofar_rec[which] then
      warn("recursive include:",which)
      return ""
    end
    if #section == 0 then
      warn("section is empty:",which)
      return ""
    end
    sofar_rec[which] = true
    sections_usecnt[which] = sections_usecnt[which] + 1

    local oldfile = FILE
    FILE = '<output>:'..which
    for i=1,#section do
      LINE=i
      --FIXME: context  trace("generate", section[i].TEXT)
      --TODO: docme actions
      local genfunc = generators[section[i].OP]
      if genfunc then
        text = text .. genfunc(section[i])
      else
        warn("no generator function for:", section[i].OP)
      end
    end
    FILE = oldfile
    sofar_rec[which] = nil
  else
    warn("no section named:", which)
  end
  return text
end


setup()
process_inputs()

FILE = "<output>"
LINE = 0

-- initialize orphans / doublettes checker
for k,_ in pairs(sections) do
  if #sections[k] > 0 then
    sections_usecnt[k] = 0
  end

  if next(sections[k].keys) then
    sections_keys_usecnt[k] = 0
  end
end

io.write(generate_output(opt_toplevel))

LINE = 0
-- orphans / doublettes
for k,v in pairs(sections_usecnt) do
  if v == 0 then
    warn("section unused:", k)
  elseif v > 1 then
    warn("section multiple times used:", k, v)
  end
end

for k,v in pairs(sections_keys_usecnt) do
  if v == 0 then
    warn("section w/ keys unused:", k)
  elseif v > 1 then
    warn("section w/ keys multiple times used:", k, v)
  end
end



--MAIN:
--: pipadoc - Documentation extractor
--: =================================
--: Christian Thaeter <ct@pipapo.org>
--: {$DATE}
--:
--:
--: Introduction
--: ------------
--:
--: Embedding documentation in program source files often yields the problem that the
--: structure of a program is not the optimal structure for the associated documentation.
--: Still there are many good reasons to maintain documentation together with the source right within
--: the code which defines the documented functionality. Pipadoc addresses this problem by extracting
--: special comments out of a source file and let one define rules how to compile the
--: documentation into proper order.  This is somewhat similar to ``literate Programming'' but
--: it puts the emphasis back to the code.
--:
--: Pipadoc is programming language and documentation system agnostic, all it requires is that
--: the programming language has some form of comments starting with a defined character sequence
--: and spaning to the end of the source line. Moreover documentation parts can be written in plain text
--: files aside from the sources.
--:
--:
--: History
--: -------
--:
--: This 'pipadoc' follows an earlier implementation with a slightly different (incompatible) syntax
--: and less features which was implemented in AWK. Updating to the new syntax should be straightforward
--: and is suggested for any projects using pipadoc.
--:
--:
--: Getting the Source
--: ------------------
--:
--: Pipadoc is managed the git revision contol system. You can clone the repository with
--:
--:  git clone --depth 1 git://git.pipapo.org/pipadoc
--:
--: The 'master' branch will stay stable and developement will be done on the 'devel' branch.
--:
--:
--: Installation
--: ------------
--:
--: Pipadoc is single lua source file `pipadoc.lua` which is portable among most Lua versions
--: (PUC Lua 5.1, 5.2, 5.3 and Luajit). It ships with a `pipadoc.install` shell script which figures a
--: suitable Lua version out and installs `pipadoc.lua` as `pipadoc` in a given directory or the current
--: directory by default.
--:
--: There are different ways how this can be used in a project:
--:
--: - One can rely on a pipadoc installed in $PATH and just call that from the build toolchain
--: - When a installed Lua version is known from the build toolchain one can include the `pipadoc.lua`
--:   into the project and call it with the known Lua interpreter.
--: - One can ship the `pipadoc.lua` and `pipadoc.install` and do a local install in the build
--:   directory and use this pipadoc thereafter
--:
--: Pipadoc tries to load 
--:
--:
--: Usage
--: -----
--:
--=usage
--:
--:
--: Basic concepts
--: --------------
--:
--: Pipadoc is controlled by special line comments. This is choosen because the most common denominator
--: between almost all programming languages is that they have some form of 'line comment', that is some
--: character sequence which defines the rest of the line as comment.
--:
--: This line comments are enhanced by a simple syntax to make them pipadoc comments. The comment
--: character sequence followed directly (without any extra space character) by some definition (see below)
--: becomes a pipadoc comment.
--:
--: Pipadoc operates in some phases. First all given files are read and parsed and finally the output is
--: generated by bringing all accumulated documentation parts together into proper order.
--:
--: When the 'strsubst' lua package is available, pipadoc adds uses this to add some extra functionality.
--:
--: [[syntax]]
--: Pipadoc Syntax
--: ~~~~~~~~~~~~~~
--:
--: Any 'linecomment' of the programming language directly (without spaces) followed by a optional
--: alphanumeric section name, followed by an operator, followed by an optional argument and then the
--: documentaton text. Only lines qualitfy this syntax are processed as pipapdoc documentation.
--:
--: The formal syntax looks like:
--:
--:  pipadoc = [source] <linecomment> [section] <operator> [arg] [..space.. [documentation_text]]
--:
--:  source = ..any sourcecode text..
--:
--:  linecomment = ..the linecomment sequence choosen by the filetype..
--:
--:  section = ..alphanumeric text including underscore and dots, but without spaces..
--:
--:  operator = [:=@#] by default, one of the defined operators
--:
--:  arg = ..alphanumeric text including underscore and dots, but without spaces..
--:
--:  documentation_text = ..rest of the line, freeform text..
--:
--:
--: Documentation lines are proccessed according to their operator.
--:
--TODO: docme oneline vs block, default section name, MAIN section
--TODO: note that literal strings are not special
--:
--: Order of operations
--: ~~~~~~~~~~~~~~~~~~~
--:
--: Pipadoc parse each file given on the commandline in order. Only lines which contain
--: pipadoc comments (see <<syntax>> above) are used in any further steps.
--: Reading files is done on a line by line base.
--:
--: After all files are read the output is generated by starting assembling the toplevel section
--: ('MAIN' if not otherwise defined). 'Strsubst' may be used to expand each generated line.
--:
--:
--: Sections and Keys
--: -----------------
--:
--=sections
--:
--:
--: String Substitation
--: -------------------
--:
--TODO: DOCME
--:
--:
--: Filetypes
--: ---------
--:
--=filetypes
--TODO: optarg
--:
--: pipadoc has builtin support for following languages
--: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--:
--@filetypes_builtin
--:
--:
--: Operators
--: ---------
--:
--=op
--TODO: optarg
--:
--: Built in operators
--: ~~~~~~~~~~~~~~~~~~
--:
--=op_builtin
--:
--:
--: [[docvars]]
--: Documentation Variables
--: -----------------------
--:
--: The 'docvars' Lua table holds key/value pairs of variables with the global state
--: of pipadoc. These can be used by the core and plugins in various ways. Debugging
--: for example prints the FILE:LINE processed and there is the 'varsubst' processor
--: to substitute them in the documentation text. The user can set arbitary docvars
--: from commandline.
--TODO: optarg
--:
--: Predefined docvars
--: ~~~~~~~~~~~~~~~~~~
--:
--@docvars
--:
--:
--: Programming API for extensions
--: ------------------------------
--:
--: Some pipadoc lua functions are documented here to be used from plugins.
--:
--=api
--:
--:
--: How to generate the pipadoc documentation itself
--: ------------------------------------------------
--:
--: 'pipadoc' documents itself with embedded asciidoc text. This can be extracted with
--:
--: ----
--: lua pipadoc.lua -m asciidoc -d pipadoc.lua >pipadoc.txt
--: ----
--:
--: The resulting `pipadoc.txt` can then be processed with the asciidoc toolchain to produce
--: distribution formats:
--:
--: -----
--: # generate HTML
--: asciidoc -a toc pipadoc.txt
--:
--: # generate PDF
--: a2x -L -k -v --dblatex-opts "-P latex.output.revhistory=0" pipadoc.txt
--: ----
--:
--: GNU General Public License
--: --------------------------
--:
--: ----
--=license
--: ----
--:
--: License Explanation
--: ~~~~~~~~~~~~~~~~~~~
--:
--: The License (GPL) only applies to pipadoc and any derivative work. The purpose of pipadoc
--: is to extract documentation from other files, this does not imply that these source files
--: from which the documentation is extracted need to be licensed under the GPL, neither does
--: this imply that the extracted documentaton need to be licensed under the GPL.
--: Using pipadoc for propietary software poses no problems about the Licenensing terms of
--: this software.
--:
--: The GPL applies when you distribute pipadoc itself, in original or modified form. Since
--: pipadoc is written in Lua you already distribute its source as well, which makes this
--: distribution naturally with the GPL.
--:
--: Nevertheless, when you make any improvements to pipadoc please consider to contact
--: Christian Thäter <ct@pipapo.org> for including them into the mainline.
--:
--ISSUES:
--:
--: ISSUES
--: ------
--:
--: ASSIGNED
--: ~~~~~~~~
--:
--@ASSIGNED
--=ASSIGNED
--:
--: FIXME
--: ~~~~~
--:
--=FIXME
--:
--FIXME: only generate FIXME Section when there are FIXME's
--:
--: TODO
--: ~~~~
--:
--=TODO
--:
--TODO: only generate TODO section when there are TODO's
--:
--: PLANNED
--: ~~~~~~~
--:
--=PLANNED
--:
--PLANNED: only generate PLANNED section when there are PLANNED's
--:


--TODO: asciidoc //source:line// comments like old pipadoc
--TODO: integrate old pipadoc.txt documentation
--PLANNED: not only pipadoc.conf but also pipadoc.sty templates, conf are local only configurations, .sty are global styles
--PLANNED: how to join (and then wordwrap) lines?
--PLANNED: bash like parameter expansion, how to apply that to sections/keys too --%{section}:%{key}
--PLANNED: org-mode processor
--PLANNED: INIT section initialize strsubst etc
--ASSIGNED:ct PLANNED: merge docvars and context to one table


--TODO: special sections
--TODO: CONFIG:PRE
--TODO: CONFIG:POST
--TODO: CONFIG:GENERATE
--PLANNED: include operator
--PLANNED: include from strsubst/config? how?


