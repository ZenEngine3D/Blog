using Sharpmake;
//using System;

namespace Samples
{
	public class Sample_Common : Project
	{
		public const string RootDir = @"[project.SharpmakeCsPath]\..\";

		public Sample_Common(string ProjectName)
		{
			Name = ProjectName;
			AddTargets(new Target(Platform.win64, DevEnv.vs2015 | DevEnv.vs2017 | DevEnv.make, Optimization.Debug | Optimization.Release));
			SourceRootPath = RootDir + ProjectName;
			IsFileNameToLower = false;
			IsTargetFileNameToLower = false;			
		}

		[Configure()]
		public void ConfigureAll(Configuration conf, Target target)
		{
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]";
			conf.ProjectPath = RootDir + @"\_Projects\[project.Name]";
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

