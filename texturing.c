#include "texturing.h"
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

//Создать текстуру из изображения. На вход подаём путь до изображения и адресс переменной unsigned int, в которую запишем готовую текстуру.
void createTexture(char* path, unsigned int *texture)
{
    int width, height, nrChannels;

    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);  //Считываем данные изображения в память

    glGenTextures(1, texture);                                               //Генерируем текстуру из указанной переменной unsigned int
    glBindTexture(GL_TEXTURE_2D, *texture);                                  //Биндим её, чтобы последующие текстурные команды применялись к ней

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, nrChannels == 4? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data); //Совмещаем данные изображения и созданную текстуру
    //1. Применить к забинженной 2D текстуре
    //2. К какому уровню mipmap хотим применить текстуру (нет mimmap)
    //3. В каком формате хотим хранить цвета
    //4-5. Ширина и высота изображения
    //6. "Always zero because some legacy stuff"
    //7-8. Формат загружаемого изображения (RGB или RGBA, unsigned_byte он же unsigned_char)
    //9. Cобственно данные изображения

    glBindTexture(GL_TEXTURE_2D, 0);                                        //Убираем бинд с текстуры

    stbi_image_free(data);                                                  //Освобождаем память, занятую данными изображения
}

//Отрендерить изображение указанного размера в указанной точке с указанной текстурой.
void renderImage (float width, float height, float xPosition, float yPosition, unsigned int texture)
{
    float vertices[] = {
         // positions                                   // texture coords
         xPosition + width, yPosition + height, 0.0f,   1.0f, 1.0f,    // top right
         xPosition + width, yPosition,          0.0f,   1.0f, 0.0f,    // bottom right
         xPosition,         yPosition,          0.0f,   0.0f, 0.0f,    // bottom left
         xPosition,         yPosition + height, 0.0f,   0.0f, 1.0f     // top left
    };

    glEnable(GL_TEXTURE_2D);                    //Включить использование 2D текстур
    glBindTexture(GL_TEXTURE_2D, texture);      //Биндим текстуру, чтобы все тестурные команды применялись конкретно к ней
    glEnable(GL_ALPHA_TEST);                    //Вклюяить учёт прозрачности
    glAlphaFunc(GL_GREATER, 0.5);               //Удалять все части текстуры с прозрачностью больше 0.5

    glPushMatrix();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        //Олна вершина содержит 5 значений (3 позиции, 2 текстурных позиции), поэтому sride = 5 * размер float. Текстуры идут со смещением на 3, так как начинаются с 3 позиции.
        glVertexPointer(2, GL_FLOAT, 5 * sizeof(float), vertices);
        glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), vertices + 3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);    //Рисуем

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
    glDisable(GL_ALPHA_TEST);                   //Выключаем всё, что включили
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

//Отрендерить изображение в соответствии с указанной матрицей вершин, та же функция что и выше, но позволяет гибче настроить рендер изображения (для анимации, например)
void renderImageFromMatrix (float vertices[], unsigned int texture)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);

    glPushMatrix();

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 5 * sizeof(float), vertices);
        glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), vertices + 3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}



