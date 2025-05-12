local directories = os.matchdirs("./*")

for _, dir in ipairs(directories) do
    local filePath = dir .. "/config.lua"

    if os.isfile(filePath) then
        include(filePath)
    end
end
