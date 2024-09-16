#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <tuple>
#include <vector>

class GraphGUI {
   public:
    GraphGUI(const std::vector<sf::Vector2f>& vertices, const std::vector<std::tuple<int, int, int, int>>& edges,
             const std::vector<std::tuple<int, int, int, int>>& mstEdges)
        : vertices(vertices), edges(edges), mstEdges(mstEdges) {
        // Load the font for displaying text
        if (!font.loadFromFile("Roboto-Bold.ttf")) {
            throw std::runtime_error("Unable to load font!");
        }
    }

    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Graph with MST");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.clear(sf::Color::White);

            // Draw all graph edges (non-MST in blue)
            for (const auto& edge : edges) {
                int from, to, weight, id;
                std::tie(from, to, weight, id) = edge;

                sf::Vector2f fromPos = vertices[from];
                sf::Vector2f toPos = vertices[to];

                if (!isEdgeInMST(from, to)) {  // Only draw non-MST edges here
                    sf::VertexArray line(sf::Lines, 2);
                    line[0].position = fromPos;
                    line[0].color = sf::Color::Blue;
                    line[1].position = toPos;
                    line[1].color = sf::Color::Blue;
                    window.draw(line);

                    // Draw edge weight for non-MST edges
                    drawEdgeWeight(window, fromPos, toPos, weight);
                }
            }

            // Highlight MST edges in bold red
            for (const auto& edge : mstEdges) {
                int from, to, weight, id;
                std::tie(from, to, weight, id) = edge;

                sf::Vector2f fromPos = vertices[from];
                sf::Vector2f toPos = vertices[to];

                drawBoldEdge(window, fromPos, toPos, sf::Color::Blue);

                // Draw edge weight for MST edges
                drawEdgeWeight(window, fromPos, toPos, weight);
            }

            // Draw vertices as circles and their corresponding vertex numbers
            for (size_t i = 0; i < vertices.size(); ++i) {
                sf::CircleShape shape(15.f);
                shape.setPosition(vertices[i].x - shape.getRadius(), vertices[i].y - shape.getRadius());
                shape.setFillColor(sf::Color::White);
                shape.setOutlineColor(sf::Color::Black);
                shape.setOutlineThickness(2.f);
                window.draw(shape);

                // Draw vertex number next to the vertex
                drawVertexNumber(window, vertices[i], i);
            }

            window.display();
        }
    }

   private:
    std::vector<sf::Vector2f> vertices;
    std::vector<std::tuple<int, int, int, int>> edges;
    std::vector<std::tuple<int, int, int, int>> mstEdges;
    sf::Font font;

    // Helper function to check if an edge is part of the MST
    bool isEdgeInMST(int from, int to) {
        for (const auto& edge : mstEdges) {
            int mst_from, mst_to, weight, id;
            std::tie(mst_from, mst_to, weight, id) = edge;
            if ((from == mst_from && to == mst_to) || (from == mst_to && to == mst_from)) {
                return true;
            }
        }
        return false;
    }

    // Helper function to draw a bold (thicker) edge
    void drawBoldEdge(sf::RenderWindow& window, sf::Vector2f from, sf::Vector2f to, sf::Color color) {
        sf::RectangleShape boldLine;
        boldLine.setSize(sf::Vector2f(distance(from, to), 6.f));  // Bold by increasing thickness
        boldLine.setFillColor(color);

        // Rotate line to match direction
        float angle = atan2(to.y - from.y, to.x - from.x) * 180 / 3.14159f;
        boldLine.setRotation(angle);
        boldLine.setPosition(from);

        window.draw(boldLine);
    }

    // Helper function to calculate the distance between two points
    float distance(sf::Vector2f point1, sf::Vector2f point2) {
        return sqrt((point2.x - point1.x) * (point2.x - point1.x) +
                    (point2.y - point1.y) * (point2.y - point1.y));
    }

    // Helper function to draw vertex number
    void drawVertexNumber(sf::RenderWindow& window, sf::Vector2f position, int vertexNumber) {
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(vertexNumber));
        text.setCharacterSize(22);
        text.setFillColor(sf::Color::Black);
        text.setPosition(position.x -5 , position.y -12);  // Offset the number a little from the vertex

        window.draw(text);
    }

    // Helper function to draw edge weight at the midpoint of an edge
    void drawEdgeWeight(sf::RenderWindow& window, sf::Vector2f from, sf::Vector2f to, int weight) {
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(weight));
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Black);

        // Calculate midpoint
        sf::Vector2f midpoint((from.x + to.x) / 2.f, (from.y + to.y) / 2.f);
        text.setPosition(midpoint.x -10, midpoint.y+15);


        window.draw(text);
    }

    
};
