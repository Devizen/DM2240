function SaveKey(outputString, overwrite)
   print("<Updating key.>")
   local f;						--File.
   if overwrite == 1 then		--Wipe the contents with new data.
      f = assert(io.open("Lua/Keyboard.lua", "w"))
   elseif overwrite == 0 then	--Append with new data.
      f = assert(io.open("Lua/Keyboard.lua", "a"))
   end
   --Write to the file.
   f:write(outputString)
   --Close the file.
   f:close()
   print("OK")
end