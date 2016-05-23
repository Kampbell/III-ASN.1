
/FROM H235-SECURITY-MESSAGES/a\
		MultimediaSystemControlMessage,\
		OpenLogicalChannel,

s/FROM MULTIMEDIA-SYSTEM-CONTROL;/FROM MULTIMEDIA-SYSTEM-CONTROL\
		PackagesDescriptor,\
		SignalsDescriptor\
	FROM MEDIA-GATEWAY-CONTROL\
		H4501SupplementaryService\
	FROM H4501-Supplementary-ServiceAPDU-Structure;/		

s/fastStart[	]*SEQUENCE OF OCTET STRING OPTIONAL/fastStart		SEQUENCE OF TYPE-IDENTIFIER\.\&Type(OpenLogicalChannel) OPTIONAL/g
s/h245Control[	]*SEQUENCE OF OCTET STRING OPTIONAL/h245Control		SEQUENCE OF TYPE-IDENTIFIER\.\&Type(MultimediaSystemControlMessage) OPTIONAL/
s/h4501SupplementaryService	SEQUENCE OF OCTET STRING OPTIONAL/h4501SupplementaryService SEQUENCE OF TYPE-IDENTIFIER\.\&Type(H4501SupplementaryService) OPTIONAL/g
s/H248PackagesDescriptor	::=	OCTET STRING/H248PackagesDescriptor	::=	TYPE-IDENTIFIER\.\&Type(PackagesDescriptor)/
s/H248SignalsDescriptor	::=	OCTET STRING/H248SignalsDescriptor	::=	TYPE-IDENTIFIER\.\&Type(SignalsDescriptor)/