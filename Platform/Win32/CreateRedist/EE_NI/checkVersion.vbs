Function CheckVersionNI()
	set WSHShell = CreateObject("WScript.Shell")
	arrMinVersion = Split("1.2.0.4", ".") 	  
	currOpenNiVer = Session.Property("OPENNIVERSION")
	
	if currOpenNiVer = "" Then
	    	WSHShell.PopUp("Please install OpenNI version 1.2.0.4 or higher!")
			CheckVersionNI = 3
			Exit Function
	End If
	arrCurrOpenNiVer = Split(currOpenNiVer, ".")

	For index = 0 to 4 
		if CInt(arrMinVersion(index)) > CInt(arrCurrOpenNiVer(index)) Then
			WSHShell.PopUp("Please install OpenNI version 1.2.0.4 and higher!")
			CheckVersionNI = 3
			Exit Function
		ElseIf CInt(arrMinVersion(index)) < CInt(arrCurrOpenNiVer(index)) Then
					CheckVersionNI = 0
					Exit Function
		End If
	Next
	CheckVerionNI = 0
End Function