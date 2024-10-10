#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

const Keyboard::Key controls[4] = {
    Keyboard::W, // Player1 UP
    Keyboard::S, // Player1 Down
    Keyboard::Up, // Player2 Up
    Keyboard::Down, // Player2 Down
};
const Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
Vector2f ballVelocity;
bool isPlayer1Serving = false;
const float initialVelocityX = 100.f;
const float initialVelocityY = 60.f;
const float velocityMultiplier = 1.1f;
bool isTwoPlayer = true; // Default to two-player mode
int player1Score = 0;
int player2Score = 0;
Font font;
Text text;

CircleShape ball;
RectangleShape paddles[2];

void Load() {
    // Load font-face from res dir
    font.loadFromFile("res/fonts/RobotoMono-Regular.ttf");
    // Set text element to use font
    text.setFont(font);
    // set the character size to 24 pixels
    text.setCharacterSize(24);
    text.setFillColor(Color::White); // Set the color

    // Initialize text positioning
    text.setPosition((gameWidth * 0.5f) - (text.getLocalBounds().width * 0.5f), 0);
    // Set size and origin of paddles
    for (auto& p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius / 2.f, ballRadius / 2.f);
    // Reset paddle position
    paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
    paddles[1].setPosition(Vector2f(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f));
    // Reset Ball Position
    ball.setPosition(Vector2f(gameWidth / 2.f, gameHeight / 2.f));
    ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };
    // Update score text immediately in Load()
    text.setString(to_string(player1Score) + " - " + to_string(player2Score));
    text.setPosition((gameWidth * 0.5f) - (text.getLocalBounds().width * 0.5f), 0);
}

void Reset() {
    // Reset paddle positions
    paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
    paddles[1].setPosition(Vector2f(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f));

    // Reset Ball Position
    ball.setPosition(Vector2f(gameWidth / 2.f, gameHeight / 2.f));

    // Reset ball velocity
    ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };

    // Update the score text after resetting
    text.setString(to_string(player1Score) + " - " + to_string(player2Score));

    // Keep score text centered
    text.setPosition((gameWidth * 0.5f) - (text.getLocalBounds().width * 0.5f), 0);
}

void Update(RenderWindow& window) {
    // Reset clock, recalculate deltatime
    static Clock clock;
    float dt = clock.restart().asSeconds();

    // Check and consume events
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
            return;
        }
    }

    ball.move(ballVelocity * dt);

    // Quit Via ESC Key
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        window.close();
    }

    // Handle paddle movement for Player 1
    float directionP1 = 0.0f;
    if (Keyboard::isKeyPressed(controls[0])) { // Player 1 UP
        directionP1--;
    }
    if (Keyboard::isKeyPressed(controls[1])) { // Player 1 Down
        directionP1++;
    }
    paddles[0].move(Vector2f(0.f, directionP1 * paddleSpeed * dt));

    // Keep Player 1 paddle within the screen bounds
    if (paddles[0].getPosition().y < paddleSize.y / 2.f) {
        paddles[0].setPosition(paddles[0].getPosition().x, paddleSize.y / 2.f);
    }
    else if (paddles[0].getPosition().y > gameHeight - paddleSize.y / 2.f) {
        paddles[0].setPosition(paddles[0].getPosition().x, gameHeight - paddleSize.y / 2.f);
    }

    // Handle paddle movement for Player 2 (or AI)
    if (isTwoPlayer) {
        float directionP2 = 0.0f;
        if (Keyboard::isKeyPressed(controls[2])) { // Player 2 UP
            directionP2--;
        }
        if (Keyboard::isKeyPressed(controls[3])) { // Player 2 Down
            directionP2++;
        }
        paddles[1].move(Vector2f(0.f, directionP2 * paddleSpeed * dt));

        // Keep Player 2 paddle within the screen bounds
        if (paddles[1].getPosition().y < paddleSize.y / 2.f) {
            paddles[1].setPosition(paddles[1].getPosition().x, paddleSize.y / 2.f);
        }
        else if (paddles[1].getPosition().y > gameHeight - paddleSize.y / 2.f) {
            paddles[1].setPosition(paddles[1].getPosition().x, gameHeight - paddleSize.y / 2.f);
        }
    }
    else {
        // AI Movement for Player 2
        float aiDirection = (ball.getPosition().y - paddles[1].getPosition().y);
        if (abs(aiDirection) > paddleSpeed * dt) {
            aiDirection = (aiDirection > 0) ? paddleSpeed * dt : -paddleSpeed * dt;
        }
        paddles[1].move(Vector2f(0.f, aiDirection));

        // Keep AI paddle within the screen bounds
        if (paddles[1].getPosition().y < paddleSize.y / 2.f) {
            paddles[1].setPosition(paddles[1].getPosition().x, paddleSize.y / 2.f);
        }
        else if (paddles[1].getPosition().y > gameHeight - paddleSize.y / 2.f) {
            paddles[1].setPosition(paddles[1].getPosition().x, gameHeight - paddleSize.y / 2.f);
        }
    }

    // Only define bx and by here, and ensure no duplicates
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;

    // Check ball collision with walls and paddles
    if (by > gameHeight) { // bottom wall
        ballVelocity.x *= velocityMultiplier;
        ballVelocity.y *= -velocityMultiplier;
        ball.move(Vector2f(0.f, -10.f));
    }
    else if (by < 0) { // top wall
        ballVelocity.x *= velocityMultiplier;
        ballVelocity.y *= -velocityMultiplier;
        ball.move(Vector2f(0.f, 10.f));
    }
    else if (bx > gameWidth) {
        // right wall
        Reset();
    }
    else if (bx < 0) {
        // left wall
        Reset();
    }
    else if (
        // ball is inline or behind left paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        // ball is below top edge of left paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5f) &&
        // ball is above bottom edge of left paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5f)
        ) {
        // Bounce off left paddle
        ballVelocity.x = -ballVelocity.x; // Reverse horizontal direction
        ball.move(Vector2f(paddleSize.x + paddleOffsetWall - bx, 0)); // Move ball outside of paddle
    }
    else if (
        // ball is inline or behind right paddle AND
        bx > gameWidth - paddleSize.x - paddleOffsetWall &&
        // ball is below top edge of right paddle AND
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5f) &&
        // ball is above bottom edge of right paddle
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5f)
        ) {
        // Bounce off right paddle
        ballVelocity.x = -ballVelocity.x; // Reverse horizontal direction
        ball.move(Vector2f(-(bx - (gameWidth - paddleSize.x - paddleOffsetWall)), 0)); // Move ball outside of paddle
    }
}

void Render(RenderWindow& window) {
    // Draw everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
}

int main() {
    RenderWindow window(VideoMode(gameWidth, gameHeight), "PONG");
    Load();
    while (window.isOpen()) {
        window.clear();
        Update(window);
        Render(window);
        window.display();
        // Draw the score text
        window.draw(text);
    }
    return 0;
}
