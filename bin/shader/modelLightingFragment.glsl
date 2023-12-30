#version 330 core
out vec4 FragColor;

// 定向光源的信息
struct DirLight {
    vec3 direction;// 光源方向

    vec3 ambient; // 环境光照强度
    vec3 diffuse;// 漫反射强度
    vec3 specular;// 镜面反射强度
};

// 点光源的信息
struct PointLight {
    vec3 position;// 点光源位置

    float constant; // Kc,通常为1.0
    float linear; // Kl 一次项会与距离值相乘，以线性的方式减少强度。
    float quadratic; // Kq 二次项会与距离的平方相乘，让光源以二次递减的方式减少强度。

    vec3 ambient;// 环境光照强度
    vec3 diffuse;// 漫反射强度
    vec3 specular;// 镜面反射强度
};

// 聚光源的信息
struct SpotLight {
    vec3 position;// 聚光源位置
    vec3 direction; //聚光源方向
    float cutOff; // 切光角
    float outerCutOff; // 外切光角

    float constant; // Kc,通常为1.0
    float linear;// Kl 一次项会与距离值相乘，以线性的方式减少强度。
    float quadratic; // Kq 二次项会与距离的平方相乘，让光源以二次递减的方式减少强度。

    vec3 ambient;// 环境光照强度
    vec3 diffuse;// 漫反射强度
    vec3 specular;// 镜面反射强度
};

#define NR_POINT_LIGHTS 3
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform bool LightStatus[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform sampler2D texture_diffuse1;// 环境光贴图
uniform sampler2D texture_specular1;// 镜面光贴图
uniform float shininess;// 影响镜面高光的散射/半径。


/**
 * 计算定向光对相应片段的贡献：它接受一些参数并计算一个定向光照颜色。
 */
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}

/**
 * 计算点光源对相应片段的贡献：它接受一些参数并计算一个定向光照颜色。
 */
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // 衰减 Fatt = 1.0 / ( Kc + Kl*d + Kq * d^2 )
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

/**
 * 计算聚光源对相应片段的贡献：它接受一些参数并计算一个定向光照颜色。
 */
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // 衰减 Fatt = 1.0 / ( Kc + Kl*d + Kq * d^2 )
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // 合并结果
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}



void main()
{
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 第一阶段：定向光照
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // 第二阶段：点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        if(LightStatus[i] == true){
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        }

    }

    // 第三阶段：聚光
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}