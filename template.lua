local insert = table.insert
local remove = table.remove
local concat = table.concat
local format = string.format
 
local loaded = {}
local pattern = "(.-){#([^#].-[^#])#}()"
 
local content = {}
local cur_content = nil
 
local function ob_start()
    cur_content = {}
    insert(content, cur_content)
end
 
local function ob_get_clean()
    local ret = concat(cur_content)
    remove(content)
    cur_content = content[#content]
    return ret
end
 
local function echo(value)
    insert(cur_content, value)
end
 
local function include(path, params)
    local bitcode = loaded[path]
 
    if not bitcode then
        local fp = io.open(path, "rb")
        local template = fp:read('*a')
        fp:close()
        local results = {}
        local last_endpos = 0
        for outside, inside, endpos in template:gmatch(pattern) do
            insert(results, format("echo(%q)", outside))
            insert(results, inside)
            last_endpos = endpos
        end
        insert(results, format("echo(%q)", template:sub(last_endpos)))
        results = concat(results, "\n")

    	local env = {
    	    include      = include,
    	    echo         = echo,
    	    ob_start     = ob_start,
    	    ob_get_clean = ob_get_clean
    	}
    	setmetatable(env, {__index = function(tb, k)
    	    return params[k] or _G[k]
    	end})

        bitcode = assert(load(results, nil, "t", env))
        loaded[path] = bitcode
    end
	bitcode()
end
 
for i = 1, 100000 do
    ob_start()
    include(arg[1], {
        params = {
            a = '1234',
            b = '4321'
        }
    })
    print(ob_get_clean())
end
