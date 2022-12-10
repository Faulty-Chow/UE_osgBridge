// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class UE_osgBridge : ModuleRules
{
	public void CopyFiles(string FromDirPath, string ToDirPath)
	{
		if(!Directory.Exists(ToDirPath))
		{
            Directory.CreateDirectory(ToDirPath);
        }
        foreach (string file in Directory.GetFiles(FromDirPath))
        {
            string ToFilePath = Path.Combine(ToDirPath, Path.GetFileName(file));
            if (!File.Exists(ToFilePath))
                File.Copy(file, ToFilePath, true);
        }
        foreach (string dir in Directory.GetDirectories(FromDirPath))
        {
            CopyFiles(dir, Path.Combine(ToDirPath, Path.GetFileName(dir)));
        }
    }


    public UE_osgBridge(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseRTTI = true;

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
				"RuntimeMeshComponent",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        PublicIncludePaths.AddRange(new string[] {
            "C:\\Users\\Administrator\\Documents\\OpenSceneGraph-3.6.5\\include",
            "C:\\Users\\Administrator\\Documents\\OpenSceneGraph-3.6.5\\build\\include",
            "C:\\Users\\Administrator\\Documents\\OpenSceneGraph-3.6.5\\3rdParty\\x64\\include"
        });

        string OSG_lib_Dir = "C:\\Users\\Administrator\\Documents\\OpenSceneGraph-3.6.5\\build\\lib";
        PublicAdditionalLibraries.AddRange(new string[] {
            Path.Combine(OSG_lib_Dir, "osg.lib"),
            Path.Combine(OSG_lib_Dir, "osgDB.lib"),
            Path.Combine(OSG_lib_Dir, "OpenThreads.lib")
        });

        //      // OpenSceneGraph-3.6.5-VC2022-64-Release
        //// Include
        //      string osgIncludePath = Path.Combine(PluginDirectory, "OpenSceneGraph-3.6.5-VC2022-64-Release/include");
        //      PublicIncludePaths.Add(osgIncludePath);
        //// Lib
        //      string osgLibPath = Path.Combine(PluginDirectory, "OpenSceneGraph-3.6.5-VC2022-64-Release/lib");
        //foreach (string lib in Directory.GetFiles(osgLibPath))
        //{
        //	PublicAdditionalLibraries.Add(lib);
        //      }
        //// Bin
        //string osgBinPath = Path.Combine(PluginDirectory, "OpenSceneGraph-3.6.5-VC2022-64-Release/bin");
        //string PluginBinariesPath = Path.Combine(PluginDirectory, "Binaries/Win64");
        //string ProjectBinariesPath = Path.Combine(PluginDirectory, "../../Binaries/Win64");
        //CopyFiles(osgBinPath, PluginBinariesPath);
        //      CopyFiles(osgBinPath, ProjectBinariesPath);
    }
}
