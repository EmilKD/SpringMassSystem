
#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>
#include<string>
#include<istream>
#include<fstream>
#include<sstream>
#include<iostream>


using std::string;

class Shader
{
public:
	unsigned int ID;
	Shader();
	void use();
	void CreateTexture(const char* filePath, string fileFormat);
	void setBool(const string &name, bool value) const;
	void setInt(const string& name, int value) const;
	void setFloat(const string& name, float value) const;

private:
	void checkCompileErrors(unsigned int shader, string type);
};

#endif // !H_SHADER