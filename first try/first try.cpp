#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <windows.h> // Para usar PlaySound

// Variables globales
int nivel = 1;
int correctas = 0;
std::vector<std::string> mensajesNiveles;
bool jugando = false;
bool mostrarMenuGanador = false;
std::vector<sf::Sprite> estrellas;

// Funciones auxiliares
std::string formatearNumero(int numero) {
    return (numero < 0) ? "(" + std::to_string(numero) + ")" : std::to_string(numero);
}

std::string generarPregunta(int& resultado) {
    int num1 = rand() % 20 - 10;
    int num2 = rand() % 20 - 10;
    int operacion = rand() % 4;

    std::ostringstream pregunta;
    switch (operacion) {
    case 0: pregunta << formatearNumero(num1) << " + " << formatearNumero(num2) << " = ?"; resultado = num1 + num2; break;
    case 1: pregunta << formatearNumero(num1) << " - " << formatearNumero(num2) << " = ?"; resultado = num1 - num2; break;
    case 2: pregunta << formatearNumero(num1) << " * " << formatearNumero(num2) << " = ?"; resultado = num1 * num2; break;
    case 3:
        if (num2 != 0 && num1 % num2 == 0) {
            pregunta << formatearNumero(num1) << " / " << formatearNumero(num2) << " = ?";
            resultado = num1 / num2;
        }
        else {
            return generarPregunta(resultado); // Recursión para evitar división por cero o no exacta
        }
        break;
    }
    return pregunta.str();
}

std::vector<int> generarOpciones(int respuestaCorrecta) {
    std::vector<int> opciones = { respuestaCorrecta };
    while (opciones.size() < 3) {
        int opcion = rand() % 41 - 20; // Rango de -20 a 20
        if (std::find(opciones.begin(), opciones.end(), opcion) == opciones.end()) {
            opciones.push_back(opcion);
        }
    }
    std::random_shuffle(opciones.begin(), opciones.end());
    return opciones;
}

void subirNivel() {
    nivel++;
    correctas = 0;
    mensajesNiveles.push_back("Nivel " + std::to_string(nivel - 1) + " completado.");
    estrellas.clear();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Juego de Matemáticas");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error cargando la fuente" << std::endl;
        return -1;
    }

    sf::Texture estrellaTextura;
    if (!estrellaTextura.loadFromFile("estrella.png")) {
        std::cerr << "Error cargando la textura de la estrella" << std::endl;
        return -1;
    }

    // Cargar la textura del fondo
    sf::Texture fondoTextura;
    if (!fondoTextura.loadFromFile("fondo.png")) {
        std::cerr << "Error cargando la textura del fondo" << std::endl;
        return -1;
    }

    // Crear un sprite para el fondo
    sf::Sprite fondoSprite(fondoTextura);

    int resultadoCorrecto;
    std::string pregunta = generarPregunta(resultadoCorrecto);
    std::vector<int> opciones = generarOpciones(resultadoCorrecto);
    int opcionSeleccionada = 0;
    std::string mensaje = "";

    sf::Text preguntaTexto(pregunta, font, 50);  // <-- Pregunta inicial
    preguntaTexto.setPosition(100, 100);
    preguntaTexto.setFillColor(sf::Color::White);

    sf::Text mensajeTexto(mensaje, font, 30);
    mensajeTexto.setPosition(100, 250);
    mensajeTexto.setFillColor(sf::Color::Black);

    sf::Text nivelCompletadoTexto("", font, 30);
    nivelCompletadoTexto.setFillColor(sf::Color::Green);

    sf::Text bienvenidaTexto("Bienvenido al Juego de Matematicas", font, 40);
    bienvenidaTexto.setPosition(100, 100);
    bienvenidaTexto.setFillColor(sf::Color::Black);

    sf::Text menuTexto("Presiona ENTER para Jugar o ESC para Salir", font, 30);
    menuTexto.setPosition(100, 250);
    menuTexto.setFillColor(sf::Color::Black);

    sf::Text menuGanadorTexto("GANASTE! Presiona BARRA ESPACIADORA para Jugar de nuevo o ESC para Salir", font, 20);
    menuGanadorTexto.setPosition(100, 400);
    menuGanadorTexto.setFillColor(sf::Color::Black);

    std::vector<sf::Text> opcionesTexto(3, sf::Text("", font, 30));
    for (int i = 0; i < 3; ++i) {
        opcionesTexto[i].setPosition(100, 300 + i * 50);
        opcionesTexto[i].setFillColor(sf::Color::White);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (!jugando && !mostrarMenuGanador) {
                    if (event.key.code == sf::Keyboard::Enter)
                        jugando = true;
                    else if (event.key.code == sf::Keyboard::Escape)
                        window.close();
                }
                else if (mostrarMenuGanador) {
                    if (event.key.code == sf::Keyboard::Space) {
                        PlaySound(NULL, 0, 0); // Detener la música actual antes de reiniciar el juego
                        mostrarMenuGanador = false;
                        jugando = true;
                        nivel = 1;
                        correctas = 0;
                        mensajesNiveles.clear();
                        estrellas.clear();
                        pregunta = generarPregunta(resultadoCorrecto);
                        opciones = generarOpciones(resultadoCorrecto);
                        preguntaTexto.setString(pregunta);  // <-- Actualizamos el texto de la pregunta
                        mensaje = "";
                    }
                    else if (event.key.code == sf::Keyboard::Escape)
                        window.close();
                }
                else if (jugando) {
                    if (event.key.code == sf::Keyboard::Down)
                        opcionSeleccionada = (opcionSeleccionada + 1) % 3;
                    else if (event.key.code == sf::Keyboard::Up)
                        opcionSeleccionada = (opcionSeleccionada + 2) % 3;
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (opciones[opcionSeleccionada] == resultadoCorrecto) {
                            mensaje = "Correcto!";
                            correctas++;

                            sf::Sprite estrella;
                            estrella.setTexture(estrellaTextura);
                            estrella.setScale(0.1f, 0.1f);
                            estrella.setPosition(static_cast<float>(50 * correctas), 50.0f); // Conversión a float
                            estrellas.push_back(estrella);

                            // Reproducir el sonido de la estrella
                            PlaySound(TEXT("estrella.wav"), NULL, SND_FILENAME | SND_ASYNC);

                            if (correctas == 5) {
                                if (nivel == 5) {
                                    mensaje = "";
                                    mostrarMenuGanador = true;
                                    jugando = false;
                                    PlaySound(TEXT("cool-guitar.wav"), NULL, SND_FILENAME | SND_ASYNC);
                                }
                                else {
                                    subirNivel();
                                }
                            }
                        }
                        else {
                            mensaje = "Incorrecto, intenta otra vez.";
                        }

                        // Actualizar pregunta y opciones
                        pregunta = generarPregunta(resultadoCorrecto);
                        opciones = generarOpciones(resultadoCorrecto);
                        preguntaTexto.setString(pregunta);  // <-- Aquí se actualiza el texto de la pregunta
                        mensajeTexto.setString(mensaje);
                        opcionSeleccionada = 0;
                    }
                }
            }

            // Detección de eventos del mouse
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (int i = 0; i < 3; ++i) {
                    if (opcionesTexto[i].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        opcionSeleccionada = i;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (opcionesTexto[opcionSeleccionada].getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    if (opciones[opcionSeleccionada] == resultadoCorrecto) {
                        mensaje = "Correcto!";
                        correctas++;

                        sf::Sprite estrella;
                        estrella.setTexture(estrellaTextura);
                        estrella.setScale(0.1f, 0.1f);
                        estrella.setPosition(static_cast<float>(50 * correctas), 50.0f); // Conversión a float
                        estrellas.push_back(estrella);

                        // Reproducir el sonido de la estrella
                        PlaySound(TEXT("estrella.wav"), NULL, SND_FILENAME | SND_ASYNC);

                        if (correctas == 5) {
                            if (nivel == 5) {
                                mensaje = "";
                                mostrarMenuGanador = true;
                                jugando = false;
                                PlaySound(TEXT("cool-guitar.wav"), NULL, SND_FILENAME | SND_ASYNC);
                            }
                            else {
                                subirNivel();
                            }
                        }
                    }
                    else {
                        mensaje = "Incorrecto, intenta otra vez.";
                    }

                    // Actualizar pregunta y opciones
                    pregunta = generarPregunta(resultadoCorrecto);
                    opciones = generarOpciones(resultadoCorrecto);
                    preguntaTexto.setString(pregunta);  // <-- Aquí se actualiza el texto de la pregunta
                    mensajeTexto.setString(mensaje);
                    opcionSeleccionada = 0;
                }
            }
        }

        window.clear();
        window.draw(fondoSprite); // Dibuja el fondo antes de los otros elementos

        if (!jugando && !mostrarMenuGanador) {
            window.draw(bienvenidaTexto);
            window.draw(menuTexto);
        }
        else if (mostrarMenuGanador) {
            window.draw(menuGanadorTexto);
        }
        else if (jugando) {
            window.draw(preguntaTexto);
            for (size_t i = 0; i < opciones.size(); ++i) {
                opcionesTexto[i].setString(std::to_string(opciones[i]));
                opcionesTexto[i].setFillColor(i == opcionSeleccionada ? sf::Color::Yellow : sf::Color::White);
                window.draw(opcionesTexto[i]);
            }

            window.draw(mensajeTexto);
            for (const auto& estrella : estrellas) {
                window.draw(estrella);
            }

            if (!mensajesNiveles.empty()) {
                nivelCompletadoTexto.setString(mensajesNiveles.back());
                window.draw(nivelCompletadoTexto);
            }
        }

        window.display();
    }

    return 0;
}