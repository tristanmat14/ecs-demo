#pragma once

#include "Application.hpp"
#include "IRenderer.hpp"
#include "Vector2.hpp"
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

// Helper functions for Vector2
namespace VectorMath {
    inline Vector2 normalize(const Vector2& v) {
        float mag = v.magnitude();
        return (mag > 0.0001f) ? v / mag : Vector2(0, 0);
    }
    
    inline float dot(const Vector2& a, const Vector2& b) {
        return a.x * b.x + a.y * b.y;
    }
    
    inline float distanceSquared(const Vector2& a, const Vector2& b) {
        Vector2 diff = b - a;
        return diff.x * diff.x + diff.y * diff.y;
    }
    
    inline float distance(const Vector2& a, const Vector2& b) {
        return std::sqrt(distanceSquared(a, b));
    }
}

struct Boid {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Color color;
    float maxSpeed;
    float maxForce;
    float perceptionRadius;
    int type; // 0=prey, 1=predator, 2=neutral
    float energy;
    bool isDead;
    
    Boid(Vector2 pos, Vector2 vel, Color col, float speed, int t)
        : position(pos), velocity(vel), acceleration(0, 0), color(col),
          maxSpeed(speed), maxForce(0.5f), perceptionRadius(50.0f),
          type(t), energy(100.0f), isDead(false) {}
    
    void applyForce(const Vector2& force) {
        acceleration += force;
    }
    
    void update(float dt) {
        velocity += acceleration * dt;
        
        // Limit velocity
        float speed = velocity.magnitude();
        if (speed > maxSpeed) {
            velocity = VectorMath::normalize(velocity) * maxSpeed;
        }
        
        position += velocity * dt;
        acceleration *= 0.0f; // Reset acceleration
        
        // Decrease energy
        energy -= dt * 2.0f;
        if (energy <= 0.0f) {
            isDead = true;
        }
    }
};

struct Food {
    Vector2 position;
    float radius;
    Color color;
    bool consumed;
    
    Food(Vector2 pos) : position(pos), radius(5.0f), 
                        color(100, 255, 100), consumed(false) {}
};

struct Obstacle {
    Vector2 position;
    float radius;
    Color color;
    
    Obstacle(Vector2 pos, float r, Color col)
        : position(pos), radius(r), color(col) {}
};

class FlockSimulationApp : public Application {
private:
    std::vector<Boid> m_boids;
    std::vector<Food> m_food;
    std::vector<Obstacle> m_obstacles;
    std::mt19937 m_rng;
    
    const float WORLD_WIDTH = 1400.0f;
    const float WORLD_HEIGHT = 900.0f;
    const size_t MAX_BOIDS = 500;
    const size_t MAX_FOOD = 200;
    
    // Flocking parameters
    const float ALIGNMENT_WEIGHT = 1.0f;
    const float COHESION_WEIGHT = 1.0f;
    const float SEPARATION_WEIGHT = 1.5f;
    const float SEPARATION_DISTANCE = 25.0f;
    
    float m_globalTime = 0.0f;
    float m_foodSpawnTimer = 0.0f;
    float m_boidSpawnTimer = 0.0f;
    int m_frameCounter = 0;
    
    // Stats
    int m_totalBoidsSpawned = 0;
    int m_preyEaten = 0;
    int m_foodEaten = 0;

public:
    explicit FlockSimulationApp(std::unique_ptr<ICore> core)
        : Application(std::move(core)), m_rng(std::random_device{}()) {}

    bool onStart() override {
        std::cout << "=== Flock Simulation with Predator-Prey Dynamics ===\n";
        std::cout << "Initializing ecosystem...\n";
        
        // Set camera: top, bottom, left, right
        getRenderer().setCameraSpace(WORLD_HEIGHT, 0, 0, WORLD_WIDTH);
        
        m_boids.reserve(MAX_BOIDS);
        m_food.reserve(MAX_FOOD);
        
        // Create obstacles
        createObstacles();
        
        // Spawn initial prey boids
        for (int i = 0; i < 50; ++i) {
            spawnBoid(0); // prey
        }
        
        // Spawn initial predators
        for (int i = 0; i < 5; ++i) {
            spawnBoid(1); // predator
        }
        
        // Spawn initial neutral boids
        for (int i = 0; i < 20; ++i) {
            spawnBoid(2); // neutral
        }
        
        // Spawn initial food
        for (int i = 0; i < 30; ++i) {
            spawnFood();
        }
        
        std::cout << "Initial boids: " << m_boids.size() << "\n";
        std::cout << "Initial food: " << m_food.size() << "\n";
        std::cout << "Obstacles: " << m_obstacles.size() << "\n";
        
        // Debug: Print first few boid positions
        std::cout << "Sample boid positions:\n";
        for (size_t i = 0; i < std::min(size_t(5), m_boids.size()); ++i) {
            std::cout << "  Boid " << i << ": (" << m_boids[i].position.x 
                      << ", " << m_boids[i].position.y << ")\n";
        }
        
        std::cout << "Simulation started!\n";
        
        return true;
    }

    void onEnd() override {
        std::cout << "\n=== Simulation Statistics ===\n";
        std::cout << "Total boids spawned: " << m_totalBoidsSpawned << "\n";
        std::cout << "Prey eaten by predators: " << m_preyEaten << "\n";
        std::cout << "Food consumed: " << m_foodEaten << "\n";
        std::cout << "Final boid count: " << m_boids.size() << "\n";
        std::cout << "Shutting down simulation...\n";
    }

    void onUpdate(float dt) override {
        dt = std::min(dt, 0.1f); // Clamp dt
        
        m_frameCounter++;
        m_globalTime += dt;
        m_foodSpawnTimer += dt;
        m_boidSpawnTimer += dt;
        
        // Spawn food periodically
        if (m_foodSpawnTimer > 0.5f && m_food.size() < MAX_FOOD) {
            for (int i = 0; i < 2; ++i) {
                spawnFood();
            }
            m_foodSpawnTimer = 0.0f;
        }
        
        // Spawn new boids occasionally
        if (m_boidSpawnTimer > 3.0f && m_boids.size() < MAX_BOIDS) {
            std::uniform_int_distribution<int> typeDist(0, 10);
            int type = (typeDist(m_rng) < 8) ? 0 : 1; // 80% prey, 20% predator
            spawnBoid(type);
            m_boidSpawnTimer = 0.0f;
        }
        
        // Update all boids with flocking behavior
        for (size_t i = 0; i < m_boids.size(); ++i) {
            if (m_boids[i].isDead) continue;
            
            Vector2 alignment = calculateAlignment(i);
            Vector2 cohesion = calculateCohesion(i);
            Vector2 separation = calculateSeparation(i);
            Vector2 avoidObstacles = calculateObstacleAvoidance(i);
            
            // Type-specific behaviors
            if (m_boids[i].type == 0) { // Prey
                Vector2 seekFood = calculateSeekFood(i);
                Vector2 fleePredators = calculateFleePredators(i);
                
                m_boids[i].applyForce(alignment * ALIGNMENT_WEIGHT);
                m_boids[i].applyForce(cohesion * COHESION_WEIGHT);
                m_boids[i].applyForce(separation * SEPARATION_WEIGHT);
                m_boids[i].applyForce(seekFood * 1.5f);
                m_boids[i].applyForce(fleePredators * 3.0f);
                m_boids[i].applyForce(avoidObstacles * 2.0f);
                
            } else if (m_boids[i].type == 1) { // Predator
                Vector2 huntPrey = calculateHuntPrey(i);
                
                m_boids[i].applyForce(separation * SEPARATION_WEIGHT * 0.5f);
                m_boids[i].applyForce(huntPrey * 2.5f);
                m_boids[i].applyForce(avoidObstacles * 2.0f);
                
            } else { // Neutral
                m_boids[i].applyForce(alignment * ALIGNMENT_WEIGHT);
                m_boids[i].applyForce(cohesion * COHESION_WEIGHT);
                m_boids[i].applyForce(separation * SEPARATION_WEIGHT);
                m_boids[i].applyForce(avoidObstacles * 2.0f);
            }
            
            // Apply boundary wrapping
            Vector2 boundaryForce = calculateBoundaryForce(i);
            m_boids[i].applyForce(boundaryForce * 2.0f);
            
            m_boids[i].update(dt);
            wrapBoid(m_boids[i]);
        }
        
        // Handle food consumption
        handleFoodConsumption();
        
        // Handle predator hunting
        handlePredatorHunting();
        
        // Remove dead entities
        m_boids.erase(
            std::remove_if(m_boids.begin(), m_boids.end(),
                [](const Boid& b) { return b.isDead; }),
            m_boids.end()
        );
        
        m_food.erase(
            std::remove_if(m_food.begin(), m_food.end(),
                [](const Food& f) { return f.consumed; }),
            m_food.end()
        );
        
        // Periodic stats
        if (m_frameCounter % 180 == 0) {
            int preyCount = 0, predatorCount = 0, neutralCount = 0;
            for (const auto& b : m_boids) {
                if (b.type == 0) preyCount++;
                else if (b.type == 1) predatorCount++;
                else neutralCount++;
            }
            
            std::cout << "[T=" << static_cast<int>(m_globalTime) << "s] "
                      << "Prey: " << preyCount 
                      << " | Predators: " << predatorCount
                      << " | Neutral: " << neutralCount
                      << " | Food: " << m_food.size() << "\n";
        }
    }

    void onRender() override {
        IRenderer& renderer = getRenderer();
        
        // Gradient background
        uint8_t bgR = static_cast<uint8_t>(15 + 10 * std::sin(m_globalTime * 0.2f));
        uint8_t bgG = static_cast<uint8_t>(25 + 10 * std::sin(m_globalTime * 0.15f));
        uint8_t bgB = static_cast<uint8_t>(40 + 15 * std::sin(m_globalTime * 0.25f));
        renderer.clearScreen(Color(bgR, bgG, bgB));
        
        // Draw boundaries - simple rectangles for each edge
        float borderThickness = 5.0f;
        renderer.drawRectangle(Vector2(0, 0), Vector2(WORLD_WIDTH, borderThickness), Color(80, 80, 100));
        renderer.drawRectangle(Vector2(0, WORLD_HEIGHT - borderThickness), Vector2(WORLD_WIDTH, WORLD_HEIGHT), Color(80, 80, 100));
        renderer.drawRectangle(Vector2(0, 0), Vector2(borderThickness, WORLD_HEIGHT), Color(80, 80, 100));
        renderer.drawRectangle(Vector2(WORLD_WIDTH - borderThickness, 0), Vector2(WORLD_WIDTH, WORLD_HEIGHT), Color(80, 80, 100));
        
        // Draw obstacles
        for (const auto& obs : m_obstacles) {
            renderer.drawCircle(obs.position, obs.radius, obs.color);
            renderer.drawCircle(obs.position, obs.radius + 2, Color(100, 100, 120));
        }
        
        // Draw food
        for (const auto& food : m_food) {
            if (food.consumed) continue;
            renderer.drawCircle(food.position, food.radius, food.color);
            renderer.drawCircle(food.position, food.radius * 1.3f, Color(80, 200, 80));
        }
        
        // Draw boids with direction indicators
        for (const auto& boid : m_boids) {
            if (boid.isDead) continue;
            
            // Energy-based color fading
            float energyFactor = std::max(0.3f, boid.energy / 100.0f);
            Color renderColor(
                static_cast<uint8_t>(boid.color.r * energyFactor),
                static_cast<uint8_t>(boid.color.g * energyFactor),
                static_cast<uint8_t>(boid.color.b * energyFactor)
            );
            
            float radius = (boid.type == 1) ? 8.0f : 5.0f; // Predators are bigger
            renderer.drawCircle(boid.position, radius, renderColor);
            
            // Draw velocity direction
            if (boid.velocity.magnitude() > 0.1f) {
                Vector2 dir = VectorMath::normalize(boid.velocity);
                Vector2 endPoint = boid.position + dir * (radius + 8.0f);
                renderer.drawLine(boid.position, endPoint, Color(255, 255, 255));
            }
        }
        
        // Draw connections between nearby boids
        int connectionCount = 0;
        for (size_t i = 0; i < m_boids.size() && connectionCount < 300; i += 3) {
            if (m_boids[i].isDead) continue;
            
            for (size_t j = i + 1; j < std::min(i + 8, m_boids.size()); ++j) {
                if (m_boids[j].isDead) continue;
                if (m_boids[i].type != m_boids[j].type) continue; // Only connect same types
                
                float distSq = VectorMath::distanceSquared(m_boids[i].position, m_boids[j].position);
                
                if (distSq < 2500.0f) { // 50 pixels
                    Color lineColor = (m_boids[i].type == 0) ? Color(100, 150, 255) : Color(255, 100, 100);
                    renderer.drawLine(m_boids[i].position, m_boids[j].position, lineColor);
                    connectionCount++;
                }
            }
        }
        
        // Debug info in first frame
        if (m_frameCounter < 5) {
            std::cout << "Frame " << m_frameCounter << " - Rendering " 
                      << m_boids.size() << " boids, " 
                      << m_food.size() << " food items, "
                      << m_obstacles.size() << " obstacles\n";
        }
    }

private:
    void createObstacles() {
        // Create scattered obstacles
        std::uniform_real_distribution<float> xDist(150.0f, WORLD_WIDTH - 150.0f);
        std::uniform_real_distribution<float> yDist(150.0f, WORLD_HEIGHT - 150.0f);
        std::uniform_real_distribution<float> radiusDist(30.0f, 60.0f);
        
        for (int i = 0; i < 8; ++i) {
            Vector2 pos(xDist(m_rng), yDist(m_rng));
            float radius = radiusDist(m_rng);
            Color color(120 + i * 10, 100, 120 - i * 5);
            m_obstacles.emplace_back(pos, radius, color);
        }
    }
    
    void spawnBoid(int type) {
        if (m_boids.size() >= MAX_BOIDS) return;
        
        std::uniform_real_distribution<float> xDist(50.0f, WORLD_WIDTH - 50.0f);
        std::uniform_real_distribution<float> yDist(50.0f, WORLD_HEIGHT - 50.0f);
        std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
        
        Vector2 pos(xDist(m_rng), yDist(m_rng));
        Vector2 vel(velDist(m_rng), velDist(m_rng));
        
        Color color;
        float maxSpeed;
        
        if (type == 0) { // Prey
            color = Color(100, 150, 255);
            maxSpeed = 150.0f;
        } else if (type == 1) { // Predator
            color = Color(255, 80, 80);
            maxSpeed = 180.0f;
        } else { // Neutral
            color = Color(200, 200, 100);
            maxSpeed = 100.0f;
        }
        
        m_boids.emplace_back(pos, vel, color, maxSpeed, type);
        m_totalBoidsSpawned++;
    }
    
    void spawnFood() {
        if (m_food.size() >= MAX_FOOD) return;
        
        std::uniform_real_distribution<float> xDist(30.0f, WORLD_WIDTH - 30.0f);
        std::uniform_real_distribution<float> yDist(30.0f, WORLD_HEIGHT - 30.0f);
        
        Vector2 pos(xDist(m_rng), yDist(m_rng));
        m_food.emplace_back(pos);
    }
    
    Vector2 calculateAlignment(size_t index) {
        Vector2 steering(0, 0);
        int total = 0;
        
        for (size_t i = 0; i < m_boids.size(); ++i) {
            if (i == index || m_boids[i].isDead) continue;
            if (m_boids[i].type != m_boids[index].type) continue;
            
            float dist = VectorMath::distance(m_boids[index].position, m_boids[i].position);
            if (dist < m_boids[index].perceptionRadius) {
                steering += m_boids[i].velocity;
                total++;
            }
        }
        
        if (total > 0) {
            steering /= static_cast<float>(total);
            steering = VectorMath::normalize(steering) * m_boids[index].maxSpeed;
            steering -= m_boids[index].velocity;
            
            // Limit force
            float mag = steering.magnitude();
            if (mag > m_boids[index].maxForce) {
                steering = VectorMath::normalize(steering) * m_boids[index].maxForce;
            }
        }
        
        return steering;
    }
    
    Vector2 calculateCohesion(size_t index) {
        Vector2 center(0, 0);
        int total = 0;
        
        for (size_t i = 0; i < m_boids.size(); ++i) {
            if (i == index || m_boids[i].isDead) continue;
            if (m_boids[i].type != m_boids[index].type) continue;
            
            float dist = VectorMath::distance(m_boids[index].position, m_boids[i].position);
            if (dist < m_boids[index].perceptionRadius) {
                center += m_boids[i].position;
                total++;
            }
        }
        
        if (total > 0) {
            center /= static_cast<float>(total);
            return seek(m_boids[index], center);
        }
        
        return Vector2(0, 0);
    }
    
    Vector2 calculateSeparation(size_t index) {
        Vector2 steering(0, 0);
        int total = 0;
        
        for (size_t i = 0; i < m_boids.size(); ++i) {
            if (i == index || m_boids[i].isDead) continue;
            
            float dist = VectorMath::distance(m_boids[index].position, m_boids[i].position);
            if (dist < SEPARATION_DISTANCE) {
                Vector2 diff = m_boids[index].position - m_boids[i].position;
                if (dist > 0.0001f) {
                    diff /= dist; // Weight by distance
                }
                steering += diff;
                total++;
            }
        }
        
        if (total > 0) {
            steering /= static_cast<float>(total);
            steering = VectorMath::normalize(steering) * m_boids[index].maxSpeed;
            steering -= m_boids[index].velocity;
            
            float mag = steering.magnitude();
            if (mag > m_boids[index].maxForce) {
                steering = VectorMath::normalize(steering) * m_boids[index].maxForce;
            }
        }
        
        return steering;
    }
    
    Vector2 calculateSeekFood(size_t index) {
        float closestDist = 1000000.0f;
        Vector2 target = m_boids[index].position;
        bool foundFood = false;
        
        for (const auto& food : m_food) {
            if (food.consumed) continue;
            
            float dist = VectorMath::distance(m_boids[index].position, food.position);
            if (dist < closestDist && dist < 200.0f) { // Only seek nearby food
                closestDist = dist;
                target = food.position;
                foundFood = true;
            }
        }
        
        if (foundFood) {
            return seek(m_boids[index], target);
        }
        
        return Vector2(0, 0);
    }
    
    Vector2 calculateFleePredators(size_t index) {
        Vector2 steering(0, 0);
        int total = 0;
        
        for (const auto& boid : m_boids) {
            if (boid.isDead || boid.type != 1) continue; // Only flee from predators
            
            float dist = VectorMath::distance(m_boids[index].position, boid.position);
            if (dist < 150.0f) { // Flee radius
                Vector2 diff = m_boids[index].position - boid.position;
                if (dist > 0.0001f) {
                    diff /= (dist * dist); // Weight heavily by distance
                }
                steering += diff;
                total++;
            }
        }
        
        if (total > 0) {
            steering /= static_cast<float>(total);
            steering = VectorMath::normalize(steering) * m_boids[index].maxSpeed;
            steering -= m_boids[index].velocity;
            
            float mag = steering.magnitude();
            if (mag > m_boids[index].maxForce * 2.0f) {
                steering = VectorMath::normalize(steering) * m_boids[index].maxForce * 2.0f;
            }
        }
        
        return steering;
    }
    
    Vector2 calculateHuntPrey(size_t index) {
        float closestDist = 1000000.0f;
        Vector2 target = m_boids[index].position;
        bool foundPrey = false;
        
        for (const auto& boid : m_boids) {
            if (boid.isDead || boid.type != 0) continue; // Only hunt prey
            
            float dist = VectorMath::distance(m_boids[index].position, boid.position);
            if (dist < closestDist && dist < 300.0f) {
                closestDist = dist;
                target = boid.position;
                foundPrey = true;
            }
        }
        
        if (foundPrey) {
            return seek(m_boids[index], target);
        }
        
        return Vector2(0, 0);
    }
    
    Vector2 calculateObstacleAvoidance(size_t index) {
        Vector2 steering(0, 0);
        
        for (const auto& obs : m_obstacles) {
            float dist = VectorMath::distance(m_boids[index].position, obs.position);
            float avoidRadius = obs.radius + 40.0f;
            
            if (dist < avoidRadius) {
                Vector2 diff = m_boids[index].position - obs.position;
                if (dist > 0.0001f) {
                    diff /= (dist * dist);
                }
                steering += diff;
            }
        }
        
        if (steering.magnitude() > 0.0001f) {
            steering = VectorMath::normalize(steering) * m_boids[index].maxSpeed;
            steering -= m_boids[index].velocity;
            
            float mag = steering.magnitude();
            if (mag > m_boids[index].maxForce * 2.0f) {
                steering = VectorMath::normalize(steering) * m_boids[index].maxForce * 2.0f;
            }
        }
        
        return steering;
    }
    
    Vector2 calculateBoundaryForce(size_t index) {
        Vector2 steering(0, 0);
        const float margin = 50.0f;
        
        if (m_boids[index].position.x < margin) {
            steering.x = m_boids[index].maxSpeed;
        } else if (m_boids[index].position.x > WORLD_WIDTH - margin) {
            steering.x = -m_boids[index].maxSpeed;
        }
        
        if (m_boids[index].position.y < margin) {
            steering.y = m_boids[index].maxSpeed;
        } else if (m_boids[index].position.y > WORLD_HEIGHT - margin) {
            steering.y = -m_boids[index].maxSpeed;
        }
        
        return steering;
    }
    
    Vector2 seek(const Boid& boid, const Vector2& target) {
        Vector2 desired = target - boid.position;
        desired = VectorMath::normalize(desired) * boid.maxSpeed;
        
        Vector2 steer = desired - boid.velocity;
        float mag = steer.magnitude();
        if (mag > boid.maxForce) {
            steer = VectorMath::normalize(steer) * boid.maxForce;
        }
        
        return steer;
    }
    
    void wrapBoid(Boid& boid) {
        if (boid.position.x < 0) boid.position.x = WORLD_WIDTH;
        if (boid.position.x > WORLD_WIDTH) boid.position.x = 0;
        if (boid.position.y < 0) boid.position.y = WORLD_HEIGHT;
        if (boid.position.y > WORLD_HEIGHT) boid.position.y = 0;
    }
    
    void handleFoodConsumption() {
        for (auto& boid : m_boids) {
            if (boid.isDead || boid.type != 0) continue; // Only prey eat food
            
            for (auto& food : m_food) {
                if (food.consumed) continue;
                
                float dist = VectorMath::distance(boid.position, food.position);
                if (dist < 10.0f) {
                    food.consumed = true;
                    boid.energy = std::min(100.0f, boid.energy + 30.0f);
                    m_foodEaten++;
                }
            }
        }
    }
    
    void handlePredatorHunting() {
        for (auto& predator : m_boids) {
            if (predator.isDead || predator.type != 1) continue;
            
            for (auto& prey : m_boids) {
                if (prey.isDead || prey.type != 0) continue;
                
                float dist = VectorMath::distance(predator.position, prey.position);
                if (dist < 15.0f) {
                    prey.isDead = true;
                    predator.energy = std::min(100.0f, predator.energy + 50.0f);
                    m_preyEaten++;
                }
            }
        }
    }
};