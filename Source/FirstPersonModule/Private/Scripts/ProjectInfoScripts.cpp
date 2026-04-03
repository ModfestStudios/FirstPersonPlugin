// Copyrighted Modfest Studios && Micah A. Parker 2025


#include "Scripts/ProjectInfoScripts.h"
#include "GeneralProjectSettings.h"

FString UProjectInfoScripts::GetProjectName()
{
	const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>();

	return ProjectSettings->ProjectName;	
}

FString UProjectInfoScripts::GetProjectDebugName()
{
	const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>();

	return ProjectSettings->ProjectDisplayedTitle.ToString();
}

FString UProjectInfoScripts::GetProjectVersion()
{
	const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>();
	
	return ProjectSettings ? ProjectSettings->ProjectVersion : FString("version not found");
}

FString UProjectInfoScripts::GetProjectDescription()
{
	const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>();

	return ProjectSettings->Description;
}

FString UProjectInfoScripts::GetProjectID()
{
	const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>();

	return ProjectSettings->ProjectID.ToString();
}
