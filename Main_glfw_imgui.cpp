//#define GLFW_INCLUDE_NONE
//#define GLFW_NOT_INITIALIZED 0x00010001

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

//#include <ImGUI/imgui.h>
//#include <ImGUI/imgui_impl_opengl3.h>
//#include <ImGUI/imgui_impl_glfw.h>

#define FENSTER_EINSTELLUNG "Fenstertitel"


/*
Kommentare:
Die nachfolgenden includes müssen für die Lauffähigkeit von OpenGL (Open Graphics Library) und GLUT (Graphics Library Utility Toolkit) in From
von dynamischen Bibliotheken (DLLs) eingebunden werden.
Dazu gehören die dynamischen Bibliotheken freeglut.dll sowie glew32.dll sowie weitere Bibliotheken
--> Projekt Rechtsklick -> Eigenschaften -> Linker -> Allgemein -> zusätzliche Bibliotheksverzeichnisse -> Bibliothekspfade angeben
--> Projekt Rechtsklick -> Eigenschaften -> Linker -> Eingabe -> Zusätzliche Abhängigkeiten -> Pfade für dynamische Bibliotheken angeben

Grundsätzliches zu der Koordinatenorientierung in OpenGL: Der Nullpunkt ist immer unten rechts, außer bei der Mauszeigerauswertung -> oben links.
Die x-Koordinate ist per default Horizontal zum Bildschirm, die y-Achse ist Senkrecht zum Bildschirm, die z-Achse sticht in den Bildschirm hinein.
*/

void InitALL(int, char* []);
void InitWindow(int, char* []);
void ResizeFunction(int, int);
void RenderFunctionDreieck(void);

void Cleanup(void);
void CreateVertexBufferDreieck(void);
void DestroyVertexBuffer(void);
void CreateShaders(void);
void DestroyShaders(void);

void error_callback(int error, const char* description);

void Meine_Maus_Aktionen(GLFWwindow* window, int button, int action, int mods);

float absToRel(int untergrenze, bool breite, int abs_Pixel_Wert);

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

int dreieck[6] = { 0, 0, 0, 0, 0, 0 };
int zaehl_global = 0;
double mouse_xPos;
double mouse_yPos;

GLuint
VertexShaderId,
FragmentShaderId,
ProgramId,
VaoId,
VboId,
ColorBufferId;

GLFWwindow* window;

const GLchar* VertexShader =
{
  "#version 400\n"\

  "layout(location=0) in vec4 in_Position;\n"\
  "layout(location=1) in vec4 in_Color;\n"\
  "out vec4 ex_Color;\n"\

  "void main(void)\n"\
  "{\n"\
  "  gl_Position = in_Position;\n"\
  "  ex_Color = in_Color;\n"\
  "}\n"
};

const GLchar* FragmentShader =
{
  "#version 400\n"\

  "in vec4 ex_Color;\n"\
  "out vec4 out_Color;\n"\

  "void main(void)\n"\
  "{\n"\
  "  out_Color = ex_Color;\n"\
  "}\n"
};

int main(int argc, char* argv[])
{
    std::cout << "main EN" << std::endl;
    InitALL(argc, argv);

    //glfwSetMouseButtonCallback(window, Meine_Maus_Aktionen);

    std::cout << "GLFW_NOT_INITIALIZED: " << GLFW_NOT_INITIALIZED << std::endl;
    std::cout << "window: " << window << std::endl;

    std::cout << "Checkpoint X3" << std::endl;
    std::cout << "glfwWindowShouldClose(window): - " << glfwWindowShouldClose(window) << std::endl;
    std::cout << "Checkpoint X4" << std::endl;

    while(!glfwWindowShouldClose(window))
    {
        std::cout << "Checkpoint while anfang" << std::endl;
        // Specify the color of the background
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Clean the back buffer and assign the new color to it
        glClear(GL_COLOR_BUFFER_BIT);
        // Tell OpenGL which Shader Program we want to use
        glUseProgram(ProgramId);
                
        // Draw primitives, number of indices, datatype of indices, index of indices

        RenderFunctionDreieck();

        // Take care of all GLFW events
        glfwPollEvents();
    }

    glfwTerminate();
    glfwDestroyWindow(window);
    
    exit(EXIT_SUCCESS);

    std::cout << "main EX" << std::endl;

    return 0;
}

void InitALL(int argc, char* argv[])
{
    std::cout << "InitALL EN" << std::endl;
    InitWindow(argc, argv);

    CreateShaders();
    /*----- Vertex Buffer erzeugen -----*/
    CreateVertexBufferDreieck();        // Beim erzeugen des Vertex Buffer MÜSSEN die Grund-Parameter bereits gesetzt sein!!!

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    std::cout << "InitALL EX" << std::endl;
}

void InitWindow(int argc, char* argv[])
{
    std::cout << "InitWindow EN" << std::endl;
    // Framework glfw initialisieren
    glfwInit();

    // Die Version von glfw mitteilen
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Profil festlegen, was den Funktionsumfang von glfw festlegt
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Ein Fenster erzeugen mit der Größe 800 x 800
    GLFWwindow* window = glfwCreateWindow(800, 800, "Mein Titel", NULL, NULL);
    // Prüfen, ob das Fenster-Onbjekt instanziiert werden konnte
    if (!window)
    {
        std::cout << "Fenster konnte nicht instanziiert werden" << std::endl;
        glfwTerminate();
    }

    // Einen Thread erzeugen, der das angegebene Fenster als Context auffasst. Bei Übergabe an einen neuen Thread muss das Fenster vom alten immer zuerst gelöst werden.
    glfwMakeContextCurrent(window);

    // GLAD in Opengl laden: mit GLAD können Funktionspointer auf OpenGL-Funktionen während der Laufzeit geladen werden.
    gladLoadGL();
    //gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))      
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }                                                    

    // Specify the viewport of OpenGL in the Window
    // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, 800, 800);
    std::cout << "InitWindow EX" << std::endl;
}

void Meine_Maus_Aktionen(GLFWwindow* window, int button, int action, int mods) {

    std::cout << "Maus Aktionen EN" << std::endl;
    
    switch (action) {    // Switch für das Drücken (!) der Maustaste [state]
    case GLFW_PRESS:

        glfwGetCursorPos(window, &mouse_xPos, &mouse_yPos);

        std::cout << "mouse_xPos: " << mouse_xPos << "; mouse_yPos: " << mouse_yPos << std::endl;

        dreieck[zaehl_global] = (int)mouse_xPos;       // explizites casting auf int
        dreieck[zaehl_global + 1] = (int)mouse_yPos;

        zaehl_global = zaehl_global + 2;

        if (zaehl_global > 4)
        {
            std::cout << "Neuer Buffer wird erzeugt!" << std::endl;

            CreateVertexBufferDreieck();

            for (int i = 0; i < 6; i++) {
                std::cout << dreieck[i] << ", ";
                std::cout << " \n";
            }
            zaehl_global = 0;

        }

        break;
    }

    std::cout << "Maus Aktionen EX" << std::endl;

}

float absToRel(int untergrenze, bool breite, int abs_Pixel_Wert) {
    std::cout << "absToRel EN" << std::endl;
    int obergrenze;
    if (breite) {       // Berechnung der X-Koordinate
        obergrenze = CurrentWidth;
        int tmp_mittel = (obergrenze - untergrenze) / 2;
        float tmp_Relativ = (float)(abs_Pixel_Wert - tmp_mittel) / tmp_mittel;
        std::cout << "absToRel X-Achse EX" << std::endl;
        return tmp_Relativ;
    }
    else {              // Berechnung der Y-Koordinate
        obergrenze = CurrentHeight;
        int tmp_mittel = (obergrenze - untergrenze) / 2;
        float tmp_Relativ = (float)(abs_Pixel_Wert - tmp_mittel) / tmp_mittel;
        std::cout << "absToRel Y-Achse EX" << std::endl;
        return (-1) * tmp_Relativ;      // Koordinate muss gespiegelt werden, da Maus-detektion aud der Y-Achse gespiegelt ist
    };
    
}

/*---- - Buffer Objekt für die vertices erzeugen---- - */
void CreateVertexBufferDreieck(void)
{
    std::cout << "CreateVertexBufferDreieck EN" << std::endl;
    // Konditionen für Shapes in einem einzelnen Buffer oder neuer Buffer für jeden Shape???

    GLfloat Vertices[] = {          // Absolutkoordinaten, die mit Funktion umgerechnet und gespiegelt werden
       absToRel(0, true, dreieck[0]), absToRel(0, false, dreieck[1]), 0.0f, 1.0f,
       absToRel(0, true, dreieck[2]), absToRel(0, false, dreieck[3]), 0.0f, 1.0f,
       absToRel(0, true, dreieck[4]), absToRel(0, false, dreieck[5]), 0.0f, 1.0f
    };

    GLfloat Colors[] = {
      0.8f, 0.8f, 0.8f, 1.0f,
      0.5f, 0.5f, 0.5f, 1.0f,
      0.2f, 0.2f, 0.2f, 1.0f
    };

    GLenum ErrorCheckValue = glGetError();

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cout << "Error Callback CreateVertexBufferDreieck!" << std::endl;
        glfwSetErrorCallback(error_callback);

        exit(-1);
    }
    std::cout << "CreateVertexBufferDreieck EX" << std::endl;
}

void DestroyVertexBuffer(void)
{
    std::cout << "DestroyVertexBuffer EN" << std::endl;
    GLenum ErrorCheckValue = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cout << "Error Callback DestroyVertexBuffer!" << std::endl;
        glfwSetErrorCallback(error_callback);

        exit(-1);
    }
    std::cout << "DestroyVertexBuffer EX" << std::endl;
}

void CreateShaders(void)
{
    std::cout << "CreateShaders EN" << std::endl;
    GLenum ErrorCheckValue = glGetError();

    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);
    glUseProgram(ProgramId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cout << "Error Callback CreateShaders!" << std::endl;
        glfwSetErrorCallback(error_callback);

        exit(-1);
    }
    std::cout << "CreateShaders EX" << std::endl;
}

void DestroyShaders(void)
{
    std::cout << "DestroyShaders EN" << std::endl;
    GLenum ErrorCheckValue = glGetError();

    glUseProgram(0);

    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);

    glDeleteProgram(ProgramId);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        std::cout << "Error Callback DestroyShaders!" << std::endl;
        glfwSetErrorCallback(error_callback);

        exit(-1);
    }
    std::cout << "DestroyShaders EX" << std::endl;
}

void ResizeFunction(int Width, int Height)
{
    std::cout << "ResizeFunction EN" << std::endl;
    CurrentWidth = Width;
    CurrentHeight = Height;

    // Der Viewport definiert, welches Teilstück aus der gesamten Szene im Fenster dargestellt wird.
    glViewport(0, 0, CurrentWidth, CurrentHeight);
    std::cout << "ResizeFunction EX" << std::endl;
}

void RenderFunctionDreieck(void)
{
    std::cout << "RenderFunctionDreieck EN" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Front Buffer und Back Buffer tauschen
    glfwSwapBuffers(window);
    std::cout << "RenderFunctionDreieck EX" << std::endl;
}

void Cleanup(void)
{
    std::cout << "Cleanup EN" << std::endl;
    DestroyShaders();
    DestroyVertexBuffer();
    std::cout << "Cleanup EX" << std::endl;
}

void error_callback(int error, const char* description)
{
    std::cout << "error_callback EN" << std::endl;
    
    fprintf(stderr, "Error: %s\n", description);
    std::cout << "error_callback EX" << std::endl;
}

/*
Specifies what kind of primitives to render. Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY are accepted.
*/