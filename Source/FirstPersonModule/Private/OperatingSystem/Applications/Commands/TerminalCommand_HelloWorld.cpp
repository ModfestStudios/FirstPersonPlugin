// Fill out your copyright notice in the Description page of Project Settings.


#include "OperatingSystem/Applications/Commands/TerminalCommand_HelloWorld.h"
#include "OperatingSystem/TerminalCommand.h"
#include "OperatingSystem/Applications/TerminalApplication.h"
#include "OperatingSystem/OperatingSystem.h"

FTerminalCommandResult UTerminalCommand_HelloWorld::OnCommandExecuted(ATerminalApplication* Terminal, FTerminalCommandExecutionParameters CommandParameters)
{
    bool fileExists = true;
    AOperatingSystem* OS = Terminal->GetOperatingSystem();
    FString HomeDirectory = OS->GetHomeDirectory();
    FOperatingSystemFiles FileResult;

    //Checks if player deleted their tool
    FileResult = OS->FileSystemGetFile("", "", "Tool:helloworld");

    if (FileResult.Name == "") {
        Terminal->PrintCommonTerminalResponse(ETerminalCommonMessage::CommandNotFound, "helloworld");
        return FTerminalCommandResult();
    }

    FileResult = OS->FileSystemGetFile("", "", "PlayerPrivateKey");

    if (FileResult.Name == "") {
        Terminal->PrintToTerminal("No authorized user private key detected: unable to decrypt message",ETerminalMessageStyle::Error);
        Terminal->PrintToTerminal(showEncryptedMessage());
    }
    else {
        Terminal->PrintToTerminal("Authorized user private key detected (~/" + HomeDirectory + FileResult.Path + "/" + FileResult.Name + "): message decrypted", ETerminalMessageStyle::OK);
        Terminal->PrintToTerminal(showDecryptedMessage());
    }
	return FTerminalCommandResult();
}

FString UTerminalCommand_HelloWorld::showDecryptedMessage() const
{
    return TEXT(R"MESSAGE(
We understand that in your current state, you have many questions.

Unfortunately, we cannot answer them at this time.

Your life is in danger. It is imperative that you help us help you.

To proceed, we need details of your environment.

There is a maintenance closet within your building. Find it.

Inside will be a box.  Open it, grab the contents.

What you need is already here, though it may not appear so.

Look beyond what is visible.  Sort matters.

Begin with what occupies the most.

Then by first recorded.

The contents will open the next door of information.

Find the winning sequence.  Report it back to us.
)MESSAGE");
}

FString UTerminalCommand_HelloWorld::showEncryptedMessage() const
{
        return TEXT(R"MESSAGE(
-----BEGIN PGP MESSAGE-----
 
hQIMAz3PgQH4JT+0AQ/+Mx/WlxGC0C0F/oTdp/fndAmuXWt/Wzog3Cf42tG8ryc9
KuUE37QcXJQ/7d1AXDbOvq6hmYL+QRgVet4BahfqfBibk4ZIwT9uj2eJgdrsi9Dd
nOsM4AXIwLAZc7uGO+yYYrEHSEEdRIz4RlXz7UpTIYAxylESXBi5ElL76dm6FsSM
qvD1lodZNFkcOdZ5J+PZLurpD7Fh+9MloKI51SlaDp40d1cREp3MPffXdk5ZWzYS
avtaBD2qVj7syVuqSSzF23qP7a+sZhdtXlg1YY81Ft0f3PD4VbWZwHOSmrGKqCTA
Mg/HI6/5Lq24+ovAdPYhxpjy32CgWUBLyNaq0GYh7jA+Vn7u7bXbMz3f1Mc4CMcy
k9h3tLBB/4NvwHMFCD6Xn4GpGBetkLTc8vLpR8PbpHEfrJ/kHl0TuBDxR3Pw+2VL
16c+2ziEKDZxz+93ypvoA93V+sY4Crox7aFv5up7a4TioSHYff+6IgtuQufX4F1L
HAEuT8XouCstf79XsrY+m/mx8Vf65hFist2jIzlSp9TkInJf3fvK0WpBZ9WFhuHb
/9gf4lFP7zV0QApb4aQsMzkIL1HxsizOp+bObdZTCHn8PsIPUaercUMu/pzkZaef
cnW1GrVyLztxc7wdbT9RuG9RnEAE+WtKBjeHO79x6lWzT61pIHFwbKFZ6hD1fKfS
sAGE8iUhOo7ZoFvojA11GzcM2HQqtSjQouLh2uBvAATvRcQGRlnUWY6smBU6DY07
46IPh+B4ccVBAEkaxXi9Kk+rBYJjNmpx2bqvLva60iV6wuktCo5tMOP5EXzKWLlz
XmIL3Pc5LAi2lhuMXY9fFpv35DLvI10CwZxOwpnHqgZ953vp9V/xWsbLWANjppOQ
c54xgMiI7SMHPr86/znV8DzlYSUOp0Ot8HgJ1QbsOeP6hQIMAz3PgQH4JT+0AQ/+
KuUE37QcXJQ/7d1AXDbOvq6hmYL+QRgVet4BahfqfBibk4ZIwT9uj2eJgdrsi9Dd
nOsM4AXIwLAZc7uGO+yYYrEHSEEdRIz4RlXz7UpTIYAxylESXBi5ElL76dm6FsSM
qvD1lodZNFkcOdZ5J+PZLurpD7Fh+9MloKI51SlaDp40d1cREp3MPffXdk5ZWzYS
avtaBD2qVj7syVuqSSzF23qP7a+sZhdtXlg1YY81Ft0f3PD4VbWZwHOSmrGKqCTA
Mg/HI6/5Lq24+ovAdPYhxpjy32CgWUBLyNaq0GYh7jA+Vn7u7bXbMz3f1Mc4CMcy
k9h3tLBB/4NvwHMFCD6Xn4GpGBetkLTc8vLpR8PbpHEfrJ/kHl0TuBDxR3Pw+2VL
16c+2ziEKDZxz+93ypvoA93V+sY4Crox7aFv5up7a4TioSHYff+6IgtuQufX4F1L
HAEuT8XouCstf79XsrY+m/mx8Vf65hFist2jIzlSp9TkInJf3fvK0WpBZ9WFhuHb
/9gf4lFP7zV0QApb4aQsMzkIL1HxsizOp+bObdZTCHn8PsIPUaercUMu/pzkZaef
cnW1GrVyLztxc7wdbT9RuG9RnEAE+WtKBjeHO79x6lWzT61pIHFwbKFZ6hD1fKfS
sAGE8iUhOo7ZoFvojA11GzcM2HQqtSjQouLh2uBvAATvRcQGRlnUWY6smBU6DY07
46IPh+B4ccVBAEkaxXi9Kk+rBYJjNmpx2bqvLva60iV6wuktCo5tMOP5EXzKWLlz
XmIL3Pc5LAi2lhuMXY9fFpv35DLvI10CwZxOwpnHqgZ953vp9V/xWsbLWANjppOQ
c54xgMiI7SMHPr86/znV8DzlYSUOp0Ot8HgJ1QbsOeP6=K5xQ
-----END PGP MESSAGE-----
)MESSAGE");
}

UTerminalCommand_HelloWorld::UTerminalCommand_HelloWorld()
{
    Command = "helloworld";
}