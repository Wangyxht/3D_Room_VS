#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;// 环境光照下这个表面反射的是什么颜色，通常与表面的颜色相同。
    vec3 specular;// 在漫反射光照下表面的颜色。漫反射颜色（和环境光照一样）也被设置为我们期望的物体颜色。
    float shininess;// 影响镜面高光的散射/半径。
};

struct Light {
    vec3 position; // 光源的位置向量

    vec3 ambient;// 环境光照强度
    vec3 diffuse;// 漫反射强度
    vec3 specular;// 镜面反射强度
};


in vec3 FragPos; // 点位世界坐标
in vec3 Normal; // 法向量
in vec2 TexCoords; // 纹理坐标

uniform vec3 viewPos;// 观察位置
uniform Material material;
uniform Light light;


uniform DirLight dirLight;

void main()
{
    // 环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // 漫反射光
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // 镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}

