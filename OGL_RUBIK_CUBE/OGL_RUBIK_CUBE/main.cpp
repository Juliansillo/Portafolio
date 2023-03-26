#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <engine/shader_m.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// ANCHOR 1) Esto es importante CONSIDERA LO SIGUIENTE

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



void randoomIndex(int*); //Función para cambiar el valor randoom de los indices
void reloadVAO(int[], float*); //Cambiar el arreglo de vertices para cambiar randoom las caras 

/*Estas banderas booleanas nos servirán para activar el cambio randoom
  y para activar la animación con el input del teclado
*/
bool initAnimation = false;
bool initRandoom = false;

/*Nos van a servir para ejecutar la animación*/
float increase = 1.0f;
float amount = 0.01f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("rubikcube.vs", "rubikcube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

   
    int faces[] = {0,1,2,3,4,5};  //Arreglo Randoom Indices (Inicialmente tienen las caras en orden)
    float vertices[180] = {0};  // Mi arreglo de vertices  (6 caras * 6 vertices por cara * 5 componentes por vertice (coordenadas y coordenadas de textura)= 180)
    reloadVAO(faces, vertices); //Inicializamos los vertices con los datos de los indices del arreglo faces


    // world space positions of our cubes
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;

    //Comentamos esta linea para evitar que nos invierta la textura y que este en el orden que marcamos nuestro mapeo
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    
    //Ponemos nuetra textura en nuestra carpeta de texturas
    unsigned char* data = stbi_load("C:\\gl\\resources\\textures\\CR.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("C:\\gl\\resources\\textures\\awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

   // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
   // -------------------------------------------------------------------------------------------
   ourShader.use();
   ourShader.setInt("texture1", 0);
   ourShader.setInt("texture2", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

         // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // pass transformation matrices to the shader
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);



        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.5f, -0.5f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        
        
        
        //RANDOOM 
        if (initRandoom)
        {
            randoomIndex(faces);//Generamos el arreglo randoom
            reloadVAO(faces, vertices);//Cambiamos las coordenadas de textura de los vertices a partir de los indices randoom
            glBindBuffer(GL_ARRAY_BUFFER, VBO); //Volvemos a elazar el buffer de vertices
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);//también añadimos los vertices
            initRandoom = false; // Una vez terminado hacemos false la bandera para que ya no entre
        }
        //ANIMACIÓN
        if (initAnimation)
        {
            if (increase >= 1.5f)
            {
                increase = 1.5f;
                amount = -0.01;
            }
            else if (increase <= 0.2f)
            {
                increase = 0.2f;
                amount = 0.01;
            }

            increase += amount;
        }

        //PASAMOS LA INFO AL SHADER
        model = glm::scale(model, glm::vec3(increase));        
        ourShader.setMat4("model", model);
      

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


//Randoom faces
void reloadVAO(int faces[], float *vertices)
{
    //Creamos un arreglo bidimensional de [6 caras] y [6 vertices * 2 elementos = 12]
  float textCoords[6][12] = {
       {
            //1,
            0.0f, 0.5f,
            0.3f, 0.5f,
            0.3f, 1.0f,
            0.3f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.5f
       },
       {
           //2
           0.33f, 0.5f,
           0.66f, 0.5f,
           0.66f, 1.0f,
           0.66f, 1.0f,
           0.33f, 1.0f,
           0.33f, 0.5f
       },
       {
           //3
           0.666f, 0.5f,
           1.0f, 0.5f,
           1.0f, 1.0f,
           1.0f, 1.0f,
           0.666f, 1.0f,
           0.666f, 0.5f
       },

       {
           //4
           0.0f, 0.0f,
           0.33f, 0.0f,
           0.33f, 0.5f,
           0.33f, 0.5f,
           0.0f, 0.5f,
           0.0f, 0.0f
       },
       {
           //5
           0.33f, 0.0f,
           0.66f, 0.0f,
           0.66f, 0.5f,
           0.66f, 0.5f,
           0.33f, 0.5f,
           0.33f, 0.0f
       },
       {
           //6
           0.666f, 0.0f,
           1.0f, 0.0f,
           1.0f, 0.5f,
           1.0f, 0.5f,
           0.666f, 0.5f,
           0.666f, 0.0f
       }
    };

  /*Seteamos las coordenadas de la siguiente manera
    textCoords[ faces[0] ][n]

    donde el faces[0] nos indica que estamos accediendo al indice de la posición 0 el cual
    al haberse hecho randoom contiene el numero de la cara que vamos a dibujar (0 a 5)

    y donde [n] nos indica que se trata del vertice y nos moveremos de
    2 elementos en 2 elementos

  */
  float vbo[] = {
       -0.5f, -0.5f, -0.5f,  textCoords[faces[0]][0], textCoords[faces[0]][1],
        0.5f, -0.5f, -0.5f,  textCoords[faces[0]][2], textCoords[faces[0]][3],
        0.5f,  0.5f, -0.5f,  textCoords[faces[0]][4], textCoords[faces[0]][5],
        0.5f,  0.5f, -0.5f,  textCoords[faces[0]][6], textCoords[faces[0]][7],
       -0.5f,  0.5f, -0.5f,  textCoords[faces[0]][8], textCoords[faces[0]][9],
       -0.5f, -0.5f, -0.5f,  textCoords[faces[0]][10], textCoords[faces[0]][11],

       -0.5f, -0.5f,  0.5f,  textCoords[faces[1]][0], textCoords[faces[1]][1],
        0.5f, -0.5f,  0.5f,  textCoords[faces[1]][2], textCoords[faces[1]][3],
        0.5f,  0.5f,  0.5f,  textCoords[faces[1]][4], textCoords[faces[1]][5],
        0.5f,  0.5f,  0.5f,  textCoords[faces[1]][6], textCoords[faces[1]][7],
       -0.5f,  0.5f,  0.5f,  textCoords[faces[1]][8], textCoords[faces[1]][9],
       -0.5f, -0.5f,  0.5f,  textCoords[faces[1]][10], textCoords[faces[1]][11],

       -0.5f,  0.5f,  0.5f,  textCoords[faces[2]][0], textCoords[faces[2]][1],
       -0.5f,  0.5f, -0.5f,  textCoords[faces[2]][2], textCoords[faces[2]][3],
       -0.5f, -0.5f, -0.5f,  textCoords[faces[2]][4], textCoords[faces[2]][5],
       -0.5f, -0.5f, -0.5f,  textCoords[faces[2]][6], textCoords[faces[2]][7],
       -0.5f, -0.5f,  0.5f,  textCoords[faces[2]][8], textCoords[faces[2]][9],
       -0.5f,  0.5f,  0.5f,  textCoords[faces[2]][10], textCoords[faces[2]][11],

        0.5f,  0.5f,  0.5f, textCoords[faces[3]][0], textCoords[faces[3]][1],
        0.5f,  0.5f, -0.5f, textCoords[faces[3]][2], textCoords[faces[3]][3],
        0.5f, -0.5f, -0.5f, textCoords[faces[3]][4], textCoords[faces[3]][5],
        0.5f, -0.5f, -0.5f, textCoords[faces[3]][6], textCoords[faces[3]][7],
        0.5f, -0.5f,  0.5f, textCoords[faces[3]][8], textCoords[faces[3]][9],
        0.5f,  0.5f,  0.5f, textCoords[faces[3]][10], textCoords[faces[3]][11],

       -0.5f, -0.5f, -0.5f,  textCoords[faces[4]][0], textCoords[faces[4]][1],
        0.5f, -0.5f, -0.5f,  textCoords[faces[4]][2], textCoords[faces[4]][3],
        0.5f, -0.5f,  0.5f,  textCoords[faces[4]][4], textCoords[faces[4]][5],
        0.5f, -0.5f,  0.5f,  textCoords[faces[4]][6], textCoords[faces[4]][7],
       -0.5f, -0.5f,  0.5f,  textCoords[faces[4]][8], textCoords[faces[4]][9],
       -0.5f, -0.5f, -0.5f,  textCoords[faces[4]][10], textCoords[faces[4]][11],

       -0.5f,  0.5f, -0.5f,  textCoords[faces[5]][0], textCoords[faces[5]][1],
        0.5f,  0.5f, -0.5f,  textCoords[faces[5]][2], textCoords[faces[5]][3],
        0.5f,  0.5f,  0.5f,  textCoords[faces[5]][4], textCoords[faces[5]][5],
        0.5f,  0.5f,  0.5f,  textCoords[faces[5]][6], textCoords[faces[5]][7],
       -0.5f,  0.5f,  0.5f,  textCoords[faces[5]][8], textCoords[faces[5]][9],
       -0.5f,  0.5f, -0.5f,  textCoords[faces[5]][10], textCoords[faces[5]][11]

  };


  //Pasamos a vertices todo lo de vbo
  for (int i = 0; i < 180; i++)
      *(vertices + i) = vbo[i];
}
void randoomIndex(int *faces)
{    //randoom de 0 a 5  osea las 6 caras de nuestro cubo
    for (int i = 0; i < 6; i++)
        *(faces+i) = rand() % 5 + 0; // randoom de 0 a 5
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        initAnimation = !initAnimation; //Esto crea un toggle (osea un botón que la primera vez que se oprime es verdadero  ya segunda falso osea un interruptor
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        initRandoom = true;//Iniciamos el randoom de faces

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}