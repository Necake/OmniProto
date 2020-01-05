#version 330 core
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct PointLight {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct FlashLight {
    vec3 direction;
	vec3 position;
	float cutoff;
	float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in GS_OUT{
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
} fs_in;

uniform Material material;
uniform PointLight pointLight;
uniform DirectionalLight dirLight;
uniform FlashLight flashLight;
uniform vec3 viewPos;
uniform samplerCube skybox;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 norm = normalize(fs_in.Normal);
	vec3 result = CalcDirectionalLight(dirLight, norm, viewDir);
	result += CalcPointLight(pointLight, norm, fs_in.FragPos, viewDir);
	result += CalcFlashLight(flashLight, norm, fs_in.FragPos, viewDir);

    vec3 r = reflect(viewDir, norm);

    FragColor = vec4(mix(result, texture(skybox, r).rgb, 0.5), 1.0);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 ambient = vec3(texture(material.texture_diffuse1, fs_in.TexCoords)) * light.ambient;

	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);
	vec3 specular = spec * vec3(texture(material.texture_specular1, fs_in.TexCoords)) * light.specular;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = vec3(texture(material.texture_diffuse1, fs_in.TexCoords)) * light.ambient;

	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0), material.shininess);
	vec3 specular = spec * vec3(texture(material.texture_specular1, fs_in.TexCoords)) * light.specular;

	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + dist * light.linear + dist * dist * light.quadratic);

    return (ambient * attenuation + diffuse * attenuation + specular * attenuation);
}

vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);

}