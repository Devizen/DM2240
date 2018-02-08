function SaveToLuaFile(outputString, overwrite)
   print("SaveToLuaFile...")
   local f;						--File.
   if overwrite == 1 then		--Wipe the contents with new data.
      f = assert(io.open("Image/DM2240_Highscore.lua", "w"))
   elseif overwrite == 0 then	--Append with new data.
      f = assert(io.open("Image/DM2240_Highscore.lua", "a"))
   end
   --Write to the file.
   f:write(outputString)
   --Close the file.
   f:close()
   print("OK")
end

title = "DM2240 - Week 14 Scripting"
width = 800
height = 600

--Volume level
volumeLevel = 1.0

hello = luaAdd(100, 100, 100)
print(hello)
hello = luaAdd(200, 100, 100)
print(hello)
hello = luaAdd(300, 100, 100)
print(hello)