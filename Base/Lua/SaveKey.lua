function SaveKey(tempKeyPath, outputString, overwrite)
   print("<Updating key " .. tempKeyPath .. ".>")
   local f;						--File.
   if overwrite == 1 then		--Wipe the contents with new data.
      f = assert(io.open(tempKeyPath, "w"))
   elseif overwrite == 0 then	--Append with new data.
      f = assert(io.open(tempKeyPath, "a"))
   end
   --Write to the file.
   f:write(outputString)
   --Close the file.
   f:close()
   print(outputString .. "<Updated.>")
end