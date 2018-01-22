using Sharpmake;
using System.Collections.Generic; //List

namespace Samples
{
	public class Sample_Common : Project
	{
		public const string RootDir = @"[project.SharpmakeCsPath]\..\";
		public bool WithEASTL = true;
		
		public Sample_Common(string InProjectName, bool InWithEASTL=true)		
		{
			Name = InProjectName;
			WithEASTL = InWithEASTL;
			AddTargets(new Target(Platform.win64, DevEnv.vs2015 | DevEnv.vs2017 | DevEnv.make, Optimization.Debug | Optimization.Release));
			SourceRootPath = RootDir + @"Samples\" + InProjectName;
			IsFileNameToLower = false;
			IsTargetFileNameToLower = false;
			
			if( WithEASTL )
			{
				AdditionalSourceRootPaths.Add(RootDir + @"\EASTL\Source");
				AdditionalSourceRootPaths.Add(RootDir + @"\EASTL\Include");
			}
		}

		[Configure()]
		public void ConfigureAll(Configuration conf, Target target)
		{
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]";
			conf.ProjectPath = RootDir + @"\_Projects\[project.Name]";
			conf.IncludePaths.Add(RootDir + @"\_Projects\[project.Name]");
			
			if( WithEASTL )
			{
				conf.IncludePaths.Add(RootDir + @"\EASTL\Include");
				conf.IncludePaths.Add(RootDir + @"\EASTL\test\packages\EABase\include\Common");			
				conf.Defines.Add("_HAS_EXCEPTIONS=0");			// Exception turned off, let windows libs know
				conf.Defines.Add("EA_COMPILER_NO_NOEXCEPT=0");	// Exception turned off, no need for NoExcept in EAStl lib
			}
			//List<string> DisabledWarning = new List<string>();
			//DisabledWarning.Add("4577"); // 'noexcept' used with no exception handling mode specified;
			//conf.Options.Add(new Sharpmake.Options.Vc.Compiler.DisableSpecificWarnings(DisabledWarning.ToArray()));
		}
	}

	//==============================================================================================
	// Projects
	//==============================================================================================
	[Sharpmake.Generate]
	public class Sample001_Signal : Sample_Common 
	{ 
		public Sample001_Signal() : base("Sample001_Signal")
		{} 
	}
	
	
	//==============================================================================================
	// Solution
	//==============================================================================================
	[Sharpmake.Generate]
	public class SolutionAllSamples : Sharpmake.Solution
	{		
		public const string RootDir = @"[solution.SharpmakeCsPath]\..\";
		public SolutionAllSamples()
		{
			Name = "AllSamples";
			AddTargets(new Target(Platform.win64, DevEnv.vs2015 | DevEnv.vs2017, Optimization.Debug | Optimization.Release));
			IsFileNameToLower = false;
		}

		[Configure()]
		public void ConfigureAll(Configuration conf, Target target)
		{
 			conf.SolutionFileName = "[solution.Name]_[target.DevEnv]";
 			conf.SolutionPath = RootDir + "_Projects";

 			conf.AddProject<Sample001_Signal>(target);
		}
		[Sharpmake.Main]
		public static void SharpmakeMain(Sharpmake.Arguments arguments)
		{
			arguments.Generate<SolutionAllSamples>();		
		}
	}
}

