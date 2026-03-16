# Visual Studio 项目转换 Linux Project

**将现有的VisualGDB工程转换为Linux项目类型以加强intellisense支持而不改变VisualGDB项目属性**

当创建VisualGDB Linux 应用程序项目时，VisualGDB会默认创建一个基于Win32的项目文件（.vcxproj）  
而不是基于Linux的项目文件。这会导致IntelliSense无法正确识别Linux特定的代码和库  
从而影响代码补全和错误检测功能。为了解决这个问题，我们可以手动修改项目文件，将其转换为基于Linux的项目文件  
例如一些gcc的方言 typeof	、__atomic 参见 https://gcc.gnu.org/onlinedocs/gcc-9.1.0/gcc/C-Extensions.html#C-Extensions  
而这些方言在 Toolset IntelliSense Identifier 为 Linux 时是可以正确识别的  
此时可以在.vcxproj文件中通过添加 `<ApplicationType>Linux</ApplicationType>` 设置实现：  

 ```xml
 <PropertyGroup Label="Globals">
    ....
	<ApplicationType>Linux</ApplicationType>
	....
 </PropertyGroup>
 ```
**以下是具体步骤：**
1. 关闭Visual Studio，确保项目没有被占用。

2. 使用文本编辑器（如Notepad++或VS Code）打开解决方案文件.sln。

3. 在sln文件中找到`GlobalSection(SolutionConfigurationPlatforms) = preSolution` 节点

4. 在该节点下添加 `"Debug|x64"` `"Release|x64"` 配置项(如果已存在则忽略该步骤)
    ```text
    GlobalSection(SolutionConfigurationPlatforms) = preSolution
        ....
    	Debug|x64 = Debug|x64
    	Release|x64 = Release|x64
    	....
    EndGlobalSection
    ```

5. 找到需要转换的项目对应的GUID 
    `Project("{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}") = "ProjectName", "...ProjectName.vcxproj", "{GUID...}"`

6. 在sln文件中找到`GlobalSection(ProjectConfigurationPlatforms) = postSolution` 节点
   在该节点下为每个配置项添加对应的项目配置，建议只保留该项目的x64架构的配置项，删除其余配置项如Win32	、x86等
    ```text
   GlobalSection(ProjectConfigurationPlatforms) = postSolution
   	....
   	{PROJECT-GUID}.Debug|x64.ActiveCfg = Debug|x64
   	{PROJECT-GUID}.Debug|x64.Build.0 = Debug|x64
   	{PROJECT-GUID}.Release|x64.ActiveCfg = Release|x64
   	{PROJECT-GUID}.Release|x64.Build.0 = Release|x64
   	....
   EndGlobalSection
    ```

7. 保存并关闭sln文件。

8. 使用文本编辑器打开项目文件.vcxproj。

9. 在.vcxproj文件中找到 `<PropertyGroup Label="Globals">` 节点添加以下行：
    ```xml
    <ApplicationType>Linux</ApplicationType>
    ```

10. 找到`<ItemGroup Label="ProjectConfigurations">`节点添加以下配置项
      建议只保留Debug|x64和Release|x64删除其他配置项

    ```xml
      <ProjectConfiguration Include="Debug|x64">
     <Configuration>Debug</Configuration>
     <Platform>x64</Platform>
      </ProjectConfiguration>
      <ProjectConfiguration Include="Release|x64">
     <Configuration>Release</Configuration>
     <Platform>x64</Platform>
      </ProjectConfiguration>
    ```

11. 找到：  
         `<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">`  
         `<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">`  
     节点，将这些节点的Condition属性值进行以下修改  
     `Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'"     ---->  Condition="'$(Configuration)|$(Platform)'=='Debug|x64'"`  
     `Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">  ---->  Condition="'$(Configuration)|$(Platform)'=='Release|x64'"`  
     建议只保留Debug|x64 和 Release|x64即可

12. 在NMakeIncludeSearchPath中添加Linux头文件搜索路径 获取linux gcc默认头文件路径可以使用 以下命令
     `cat /dev/null | g++ -x c++ -E -v -`
     在输出中找到#include <...> search starts here: 即为需要复制到Windows开发环境中的includePath并将其按顺序添加到NMakeIncludeSearchPath即可

13. 添加以下两个空元素：

     ```xml
     <IncludePath />
     <ExternalIncludePath />
     ```

     

14. 保存并关闭.vcxproj文件

15. 重新使用vs打开sln以验证修改是否成功。


**本程序目的:**
* 为实现以上步骤的UI界面和自动化操作  
* 增加NMakeIncludeSearchPath目录的管理功能
* 针对不同项目提供不同系列的包含搜索路径管理功能
