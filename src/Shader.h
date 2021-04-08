#pragma once
#include<string>
#include<unordered_map>

#include"glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int>m_UniformLocationCache;
public:
	
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;
	unsigned int GetRendererID() { return m_RendererID; };
	//set uniforms
	void SetUniform4f(const std::string name, float v0, float v1, float v2, float v3);
	void SetUniform1f(const std::string name, float value);
	void SetUniform1i(const std::string name, int value);
	void SetUniformMat4f(const std::string name, const glm::mat4& matrix);

	int GetUniformLocation(const std::string& name);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	
};
