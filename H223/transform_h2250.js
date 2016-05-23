
var replace_table;
replace_table = [ [/FROM H235-SECURITY-MESSAGES/,"FROM H235-SECURITY-MESSAGES\n		MultimediaSystemControlMessage,\n		OpenLogicalChannel,"],
                  [/FROM MULTIMEDIA-SYSTEM-CONTROL;/,"FROM MULTIMEDIA-SYSTEM-CONTROL\n		PackagesDescriptor,\n		SignalsDescriptor\n	FROM MEDIA-GATEWAY-CONTROL\n		H4501SupplementaryService\n	FROM H4501-Supplementary-ServiceAPDU-Structure;"],
                  [/fastStart\s+SEQUENCE OF OCTET STRING OPTIONAL/,"fastStart		SEQUENCE OF TYPE-IDENTIFIER.&Type(OpenLogicalChannel) OPTIONAL"],
									[/h245Control\s+SEQUENCE OF OCTET STRING OPTIONAL/,"h245Control		SEQUENCE OF TYPE-IDENTIFIER\.\&Type(MultimediaSystemControlMessage) OPTIONAL"],
									[/h4501SupplementaryService	SEQUENCE OF OCTET STRING OPTIONAL/,"h4501SupplementaryService SEQUENCE OF TYPE-IDENTIFIER.&Type(H4501SupplementaryService) OPTIONAL"],
									[/H248PackagesDescriptor	::=	OCTET STRING/,"H248PackagesDescriptor	::=	TYPE-IDENTIFIER\.\&Type(PackagesDescriptor)"],
									[/H248SignalsDescriptor	::=	OCTET STRING/,"H248SignalsDescriptor	::=	TYPE-IDENTIFIER\.\&Type(SignalsDescriptor)"] ];

var fso, infile, outfile;
var ForReading = 1, ForWriting = 2;

fso = new ActiveXObject("Scripting.FileSystemObject");
infile = fso.OpenTextFile("..\\asnh323\\h2250v4.asn", ForReading);
outfile = fso.OpenTextFile("h2250v4_new.asn", ForWriting,true);

while ( infile.AtEndOfStream != true )
{
	  var str = infile.Readline();

    for(var i=0; i<replace_table.length; i++){
    	str = str.replace(replace_table[i][0],replace_table[i][1]);
    }
    outfile.WriteLine(str);
}
 