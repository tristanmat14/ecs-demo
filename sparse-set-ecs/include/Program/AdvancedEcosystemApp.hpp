#pragma once

#include "Application.hpp"
#include "IRenderer.hpp"
#include "Vector2.hpp"
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <memory>

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
    
    inline Vector2 rotate(const Vector2& v, float angle) {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return Vector2(v.x * c - v.y * s, v.x * s + v.y * c);
    }
}

// Genetic traits for evolution
struct Genes {
    float maxSpeed;
    float perceptionRadius;
    float aggression;
    float fearResponse;
    float metabolism;
    float reproductionThreshold;
    
    Genes() : maxSpeed(150.0f), perceptionRadius(50.0f), aggression(0.5f),
              fearResponse(0.5f), metabolism(1.0f), reproductionThreshold(80.0f) {}
    
    Genes mutate(std::mt19937& rng) const {
        std::uniform_real_distribution<float> mutationDist(-0.1f, 0.1f);
        Genes child = *this;
        child.maxSpeed += mutationDist(rng) * 30.0f;
        child.perceptionRadius += mutationDist(rng) * 10.0f;
        child.aggression += mutationDist(rng) * 0.2f;
        child.fearResponse += mutationDist(rng) * 0.2f;
        child.metabolism += mutationDist(rng) * 0.2f;
        child.reproductionThreshold += mutationDist(rng) * 10.0f;
        
        // Clamp values
        child.maxSpeed = std::max(50.0f, std::min(250.0f, child.maxSpeed));
        child.perceptionRadius = std::max(20.0f, std::min(100.0f, child.perceptionRadius));
        child.aggression = std::max(0.0f, std::min(1.0f, child.aggression));
        child.fearResponse = std::max(0.0f, std::min(1.0f, child.fearResponse));
        child.metabolism = std::max(0.5f, std::min(2.0f, child.metabolism));
        child.reproductionThreshold = std::max(60.0f, std::min(95.0f, child.reproductionThreshold));
        
        return child;
    }
};

struct Boid {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Color color;
    float maxForce;
    int type; // 0=herbivore, 1=carnivore, 2=omnivore, 3=scavenger
    float energy;
    float health;
    bool isDead;
    int generation;
    Genes genes;
    float reproductionCooldown;
    int age; // in frames
    bool isChild;
    
    Boid(Vector2 pos, Vector2 vel, Color col, int t, int gen = 0)
        : position(pos), velocity(vel), acceleration(0, 0), color(col),
          maxForce(0.5f), type(t), energy(100.0f), health(100.0f),
          isDead(false), generation(gen), reproductionCooldown(0.0f),
          age(0), isChild(true) {}
    
    void applyForce(const Vector2& force) {
        acceleration += force;
    }
    
    void update(float dt) {
        velocity += acceleration * dt;
        
        // Limit velocity based on genes
        float speed = velocity.magnitude();
        if (speed > genes.maxSpeed) {
            velocity = VectorMath::normalize(velocity) * genes.maxSpeed;
        }
        
        position += velocity * dt;
        acceleration *= 0.0f;
        
        // Energy consumption based on metabolism and speed
        float movementCost = (speed / genes.maxSpeed) * genes.metabolism;
        energy -= dt * (1.0f + movementCost);
        
        // Age the boid
        age++;
        if (age > 1800) { // 30 seconds at 60fps
            isChild = false;
        }
        
        // Reproduction cooldown
        if (reproductionCooldown > 0.0f) {
            reproductionCooldown -= dt;
        }
        
        // Health deterioration when energy is low
        if (energy <= 0.0f) {
            health -= dt * 10.0f;
            if (health <= 0.0f) {
                isDead = true;
            }
        } else if (energy > 50.0f && health < 100.0f) {
            health += dt * 5.0f; // Regenerate health
        }
    }
};

struct Food {
    Vector2 position;
    float radius;
    Color color;
    bool consumed;
    int foodType; // 0=plant, 1=meat (from corpse)
    float energy;
    
    Food(Vector2 pos, int type) 
        : position(pos), radius(5.0f), consumed(false), 
          foodType(type), energy(30.0f) {
        color = (type == 0) ? Color(100, 255, 100) : Color(180, 80, 80);
    }
};

struct Obstacle {
    Vector2 position;
    float radius;
    Color color;
    bool isNest; // Safe zones for reproduction
    
    Obstacle(Vector2 pos, float r, Color col, bool nest = false)
        : position(pos), radius(r), color(col), isNest(nest) {}
};

// Weather system affects behavior
struct WeatherSystem {
    float windStrength;
    Vector2 windDirection;
    float temperature; // Affects metabolism
    bool isRaining;
    float dayNightCycle; // 0-1, affects visibility
    
    WeatherSystem() : windStrength(0.0f), windDirection(1, 0), 
                      temperature(20.0f), isRaining(false), dayNightCycle(0.5f) {}
    
    void update(float dt, std::mt19937& rng) {
        dayNightCycle += dt * 0.05f;
        if (dayNightCycle > 1.0f) dayNightCycle = 0.0f;
        
        // Random weather changes
        std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
        if (chanceDist(rng) < 0.001f) {
            isRaining = !isRaining;
        }
        
        // Wind changes
        windStrength += (chanceDist(rng) - 0.5f) * 0.1f;
        windStrength = std::max(0.0f, std::min(30.0f, windStrength));
        
        float angleChange = (chanceDist(rng) - 0.5f) * 0.1f;
        windDirection = VectorMath::rotate(windDirection, angleChange);
        windDirection = VectorMath::normalize(windDirection);
        
        temperature = 20.0f + 10.0f * std::sin(dayNightCycle * 2.0f * 3.14159f);
    }
    
    Vector2 getWindForce() const {
        return windDirection * windStrength;
    }
    
    float getVisibilityModifier() const {
        // Reduced visibility at night and in rain
        float nightEffect = 0.3f + 0.7f * std::sin(dayNightCycle * 3.14159f);
        float rainEffect = isRaining ? 0.7f : 1.0f;
        return nightEffect * rainEffect;
    }
};

// Spatial partitioning for optimization
struct SpatialGrid {
    static const int CELL_SIZE = 100;
    std::vector<std::vector<int>> grid;
    int width, height;
    
    void initialize(int worldW, int worldH) {
        width = (worldW / CELL_SIZE) + 1;
        height = (worldH / CELL_SIZE) + 1;
        grid.resize(width * height);
    }
    
    void clear() {
        for (auto& cell : grid) {
            cell.clear();
        }
    }
    
    void insert(const Vector2& pos, int index) {
        int x = static_cast<int>(pos.x / CELL_SIZE);
        int y = static_cast<int>(pos.y / CELL_SIZE);
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y * width + x].push_back(index);
        }
    }
    
    std::vector<int> query(const Vector2& pos, float radius) {
        std::vector<int> result;
        int minX = std::max(0, static_cast<int>((pos.x - radius) / CELL_SIZE));
        int maxX = std::min(width - 1, static_cast<int>((pos.x + radius) / CELL_SIZE));
        int minY = std::max(0, static_cast<int>((pos.y - radius) / CELL_SIZE));
        int maxY = std::min(height - 1, static_cast<int>((pos.y + radius) / CELL_SIZE));
        
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                const auto& cell = grid[y * width + x];
                result.insert(result.end(), cell.begin(), cell.end());
            }
        }
        return result;
    }
};

class AdvancedEcosystemApp : public Application {
private:
    std::vector<Boid> m_boids;
    std::vector<Food> m_food;
    std::vector<Obstacle> m_obstacles;
    std::mt19937 m_rng;
    SpatialGrid m_spatialGrid;
    WeatherSystem m_weather;
    
    const float WORLD_WIDTH = 1600.0f;
    const float WORLD_HEIGHT = 1000.0f;
    const size_t MAX_BOIDS = 300;
    const size_t MAX_FOOD = 150;
    
    // Flocking parameters
    const float ALIGNMENT_WEIGHT = 0.8f;
    const float COHESION_WEIGHT = 0.8f;
    const float SEPARATION_WEIGHT = 1.5f;
    const float SEPARATION_DISTANCE = 25.0f;
    
    float m_globalTime = 0.0f;
    float m_foodSpawnTimer = 0.0f;
    int m_frameCounter = 0;
    
    // Stats
    int m_totalBirths = 0;
    int m_totalDeaths = 0;
    int m_generationMax = 0;
    
    // Ecosystem zones
    struct Zone {
        Vector2 center;
        float radius;
        int biomeType; // 0=forest (plants), 1=plains (mixed), 2=desert (sparse)
    };
    std::vector<Zone> m_zones;

public:
    explicit AdvancedEcosystemApp(std::unique_ptr<ICore> core)
        : Application(std::move(core)), m_rng(std::random_device{}()) {}

    bool onStart() override {
        std::cout << "=== Advanced Ecosystem Simulation ===\n";
        std::cout << "Features: Genetics, Evolution, Weather, Biomes\n";
        
        getRenderer().setCameraSpace(WORLD_HEIGHT, 0, 0, WORLD_WIDTH);
        
        m_boids.reserve(MAX_BOIDS);
        m_food.reserve(MAX_FOOD);
        m_spatialGrid.initialize(WORLD_WIDTH, WORLD_HEIGHT);
        
        // Create biome zones
        createZones();
        
        // Create obstacles and nests
        createObstacles();
        
        // Spawn initial population
        for (int i = 0; i < 30; ++i) spawnBoid(0); // Herbivores
        for (int i = 0; i < 10; ++i) spawnBoid(1); // Carnivores
        for (int i = 0; i < 15; ++i) spawnBoid(2); // Omnivores
        for (int i = 0; i < 5; ++i) spawnBoid(3);  // Scavengers
        
        // Spawn initial food
        for (int i = 0; i < 50; ++i) spawnFood(0);
        
        std::cout << "Initial population: " << m_boids.size() << "\n";
        std::cout << "Biomes: " << m_zones.size() << "\n";
        std::cout << "Simulation started!\n";
        
        return true;
    }

    void onEnd() override {
        std::cout << "\n=== Final Statistics ===\n";
        std::cout << "Total births: " << m_totalBirths << "\n";
        std::cout << "Total deaths: " << m_totalDeaths << "\n";
        std::cout << "Max generation reached: " << m_generationMax << "\n";
        std::cout << "Final population: " << m_boids.size() << "\n";
    }

    void onUpdate(float dt) override {
        dt = std::min(dt, 0.1f);
        
        m_frameCounter++;
        m_globalTime += dt;
        m_foodSpawnTimer += dt;
        
        // Update weather
        m_weather.update(dt, m_rng);
        
        // Rebuild spatial grid
        m_spatialGrid.clear();
        for (size_t i = 0; i < m_boids.size(); ++i) {
            if (!m_boids[i].isDead) {
                m_spatialGrid.insert(m_boids[i].position, i);
            }
        }
        
        // Spawn food based on biomes
        if (m_foodSpawnTimer > 0.3f && m_food.size() < MAX_FOOD) {
            for (const auto& zone : m_zones) {
                if (zone.biomeType == 0 || zone.biomeType == 1) {
                    spawnFoodInZone(zone, 0); // Plants
                }
            }
            m_foodSpawnTimer = 0.0f;
        }
        
        // Update all boids
        for (size_t i = 0; i < m_boids.size(); ++i) {
            if (m_boids[i].isDead) continue;
            
            updateBoidBehavior(i, dt);
            m_boids[i].update(dt);
            wrapBoid(m_boids[i]);
        }
        
        // Handle interactions
        handleFoodConsumption();
        handlePredation();
        handleReproduction();
        
        // Create corpses from dead boids
        for (const auto& boid : m_boids) {
            if (boid.isDead && !boid.isChild && m_food.size() < MAX_FOOD) {
                m_food.emplace_back(boid.position, 1); // Meat
            }
        }
        
        // Remove dead entities
        size_t beforeCount = m_boids.size();
        m_boids.erase(
            std::remove_if(m_boids.begin(), m_boids.end(),
                [](const Boid& b) { return b.isDead; }),
            m_boids.end()
        );
        m_totalDeaths += (beforeCount - m_boids.size());
        
        m_food.erase(
            std::remove_if(m_food.begin(), m_food.end(),
                [](const Food& f) { return f.consumed; }),
            m_food.end()
        );
        
        // Periodic stats
        if (m_frameCounter % 300 == 0) {
            printStats();
        }
    }

    void onRender() override {
        IRenderer& renderer = getRenderer();
        
        // Day/night cycle background
        float dayBrightness = 0.5f + 0.5f * std::sin(m_weather.dayNightCycle * 3.14159f);
        uint8_t bgR = static_cast<uint8_t>(10 + 40 * dayBrightness);
        uint8_t bgG = static_cast<uint8_t>(15 + 50 * dayBrightness);
        uint8_t bgB = static_cast<uint8_t>(30 + 70 * dayBrightness);
        
        if (m_weather.isRaining) {
            bgB += 20; // Bluer when raining
        }
        
        renderer.clearScreen(Color(bgR, bgG, bgB));
        
        // Draw biome zones
        for (const auto& zone : m_zones) {
            Color zoneColor;
            if (zone.biomeType == 0) zoneColor = Color(40, 80, 40); // Forest
            else if (zone.biomeType == 1) zoneColor = Color(60, 90, 50); // Plains
            else zoneColor = Color(100, 80, 50); // Desert
            
            renderer.drawCircle(zone.center, zone.radius, zoneColor);
        }
        
        // Draw boundaries
        float border = 5.0f;
        renderer.drawRectangle(Vector2(0, 0), Vector2(WORLD_WIDTH, border), Color(80, 80, 100));
        renderer.drawRectangle(Vector2(0, WORLD_HEIGHT - border), Vector2(WORLD_WIDTH, WORLD_HEIGHT), Color(80, 80, 100));
        renderer.drawRectangle(Vector2(0, 0), Vector2(border, WORLD_HEIGHT), Color(80, 80, 100));
        renderer.drawRectangle(Vector2(WORLD_WIDTH - border, 0), Vector2(WORLD_WIDTH, WORLD_HEIGHT), Color(80, 80, 100));
        
        // Draw obstacles
        for (const auto& obs : m_obstacles) {
            Color obsColor = obs.isNest ? Color(150, 120, 180) : obs.color;
            renderer.drawCircle(obs.position, obs.radius, obsColor);
            if (obs.isNest) {
                renderer.drawCircle(obs.position, obs.radius * 1.2f, Color(180, 150, 200));
            }
        }
        
        // Draw food
        for (const auto& food : m_food) {
            if (food.consumed) continue;
            renderer.drawCircle(food.position, food.radius, food.color);
            
            // Visual distinction for meat vs plants
            if (food.foodType == 1) {
                renderer.drawCircle(food.position, food.radius * 1.5f, Color(140, 60, 60));
            }
        }
        
        // Draw wind direction indicator
        if (m_weather.windStrength > 5.0f) {
            Vector2 windStart(50, 50);
            Vector2 windEnd = windStart + m_weather.windDirection * (m_weather.windStrength * 2.0f);
            renderer.drawLine(windStart, windEnd, Color(200, 200, 255));
        }
        
        // Draw boids
        for (const auto& boid : m_boids) {
            if (boid.isDead) continue;
            
            // Energy-based transparency
            float energyFactor = std::max(0.3f, boid.energy / 100.0f);
            Color renderColor(
                static_cast<uint8_t>(boid.color.r * energyFactor),
                static_cast<uint8_t>(boid.color.g * energyFactor),
                static_cast<uint8_t>(boid.color.b * energyFactor)
            );
            
            // Size based on type and age
            float radius = boid.isChild ? 3.0f : 6.0f;
            if (boid.type == 1) radius += 2.0f; // Carnivores larger
            
            renderer.drawCircle(boid.position, radius, renderColor);
            
            // Draw velocity direction
            if (boid.velocity.magnitude() > 0.1f) {
                Vector2 dir = VectorMath::normalize(boid.velocity);
                Vector2 endPoint = boid.position + dir * (radius + 6.0f);
                renderer.drawLine(boid.position, endPoint, Color(255, 255, 255));
            }
            
            // Health bar for low health boids
            if (boid.health < 50.0f) {
                Vector2 barStart = boid.position + Vector2(-8, -12);
                Vector2 barEnd = barStart + Vector2(16.0f * (boid.health / 100.0f), 0);
                renderer.drawLine(barStart, barEnd, Color(255, 0, 0));
            }
        }
        
        // Draw connections between flockmates
        int connectionCount = 0;
        for (size_t i = 0; i < m_boids.size() && connectionCount < 200; i += 4) {
            if (m_boids[i].isDead) continue;
            
            auto nearby = m_spatialGrid.query(m_boids[i].position, 60.0f);
            for (int idx : nearby) {
                if (idx <= static_cast<int>(i) || m_boids[idx].isDead) continue;
                if (m_boids[i].type != m_boids[idx].type) continue;
                
                float distSq = VectorMath::distanceSquared(m_boids[i].position, m_boids[idx].position);
                if (distSq < 3600.0f) {
                    Color lineColor;
                    if (m_boids[i].type == 0) lineColor = Color(100, 150, 255);
                    else if (m_boids[i].type == 1) lineColor = Color(255, 100, 100);
                    else if (m_boids[i].type == 2) lineColor = Color(200, 200, 100);
                    else lineColor = Color(150, 100, 200);
                    
                    renderer.drawLine(m_boids[i].position, m_boids[idx].position, lineColor);
                    connectionCount++;
                    if (connectionCount >= 200) break;
                }
            }
        }
        
        // Rain effect
        if (m_weather.isRaining) {
            std::uniform_real_distribution<float> xDist(0, WORLD_WIDTH);
            std::uniform_real_distribution<float> yDist(0, WORLD_HEIGHT);
            for (int i = 0; i < 50; ++i) {
                Vector2 rainStart(xDist(m_rng), yDist(m_rng));
                Vector2 rainEnd = rainStart + Vector2(5, 15);
                renderer.drawLine(rainStart, rainEnd, Color(150, 150, 200));
            }
        }
    }

private:
    void createZones() {
        // Create diverse biomes
        std::uniform_real_distribution<float> xDist(200.0f, WORLD_WIDTH - 200.0f);
        std::uniform_real_distribution<float> yDist(200.0f, WORLD_HEIGHT - 200.0f);
        
        for (int i = 0; i < 6; ++i) {
            Zone zone;
            zone.center = Vector2(xDist(m_rng), yDist(m_rng));
            zone.radius = 150.0f + (i * 20.0f);
            zone.biomeType = i % 3;
            m_zones.push_back(zone);
        }
    }
    
    void createObstacles() {
        std::uniform_real_distribution<float> xDist(150.0f, WORLD_WIDTH - 150.0f);
        std::uniform_real_distribution<float> yDist(150.0f, WORLD_HEIGHT - 150.0f);
        
        // Regular obstacles
        for (int i = 0; i < 10; ++i) {
            Vector2 pos(xDist(m_rng), yDist(m_rng));
            float radius = 30.0f + (i * 3.0f);
            m_obstacles.emplace_back(pos, radius, Color(100, 100, 120), false);
        }
        
        // Nests (safe zones)
        for (int i = 0; i < 4; ++i) {
            Vector2 pos(xDist(m_rng), yDist(m_rng));
            m_obstacles.emplace_back(pos, 40.0f, Color(150, 120, 180), true);
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
        if (type == 0) color = Color(100, 150, 255);      // Herbivore
        else if (type == 1) color = Color(255, 80, 80);   // Carnivore
        else if (type == 2) color = Color(200, 200, 100); // Omnivore
        else color = Color(150, 100, 200);                 // Scavenger
        
        m_boids.emplace_back(pos, vel, color, type, 0);
    }
    
    void spawnFood(int foodType) {
        if (m_food.size() >= MAX_FOOD) return;
        
        std::uniform_real_distribution<float> xDist(30.0f, WORLD_WIDTH - 30.0f);
        std::uniform_real_distribution<float> yDist(30.0f, WORLD_HEIGHT - 30.0f);
        
        m_food.emplace_back(Vector2(xDist(m_rng), yDist(m_rng)), foodType);
    }
    
    void spawnFoodInZone(const Zone& zone, int foodType) {
        if (m_food.size() >= MAX_FOOD) return;
        
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
        std::uniform_real_distribution<float> radiusDist(0.0f, zone.radius);
        
        float angle = angleDist(m_rng);
        float radius = radiusDist(m_rng);
        
        Vector2 pos = zone.center + Vector2(std::cos(angle), std::sin(angle)) * radius;
        m_food.emplace_back(pos, foodType);
    }
    
    void updateBoidBehavior(size_t index, float dt) {
        auto& boid = m_boids[index];
        
        // Get nearby boids using spatial grid
        float searchRadius = boid.genes.perceptionRadius * m_weather.getVisibilityModifier();
        auto nearbyIndices = m_spatialGrid.query(boid.position, searchRadius);
        
        // Flocking forces (only with same type)
        Vector2 alignment(0, 0);
        Vector2 cohesion(0, 0);
        Vector2 separation(0, 0);
        int flockCount = 0;
        
        for (int idx : nearbyIndices) {
            if (idx == static_cast<int>(index) || m_boids[idx].isDead) continue;
            if (m_boids[idx].type != boid.type) continue;
            
            float dist = VectorMath::distance(boid.position, m_boids[idx].position);
            if (dist < searchRadius) {
                alignment += m_boids[idx].velocity;
                cohesion += m_boids[idx].position;
                flockCount++;
            }
            
            if (dist < SEPARATION_DISTANCE) {
                Vector2 diff = boid.position - m_boids[idx].position;
                if (dist > 0.0001f) diff /= dist;
                separation += diff;
            }
        }
        
        if (flockCount > 0) {
            alignment /= static_cast<float>(flockCount);
            alignment = VectorMath::normalize(alignment) * boid.genes.maxSpeed;
            alignment -= boid.velocity;
            limitForce(alignment, boid.maxForce);
            
            cohesion /= static_cast<float>(flockCount);
            cohesion = seek(boid, cohesion);
            
            boid.applyForce(alignment * ALIGNMENT_WEIGHT);
            boid.applyForce(cohesion * COHESION_WEIGHT);
        }
        
        if (separation.magnitude() > 0.0001f) {
            separation = VectorMath::normalize(separation) * boid.genes.maxSpeed;
            separation -= boid.velocity;
            limitForce(separation, boid.maxForce);
            boid.applyForce(separation * SEPARATION_WEIGHT);
        }
        
        // Type-specific behaviors
        if (boid.type == 0) { // Herbivore
            Vector2 seekFood = findNearestFood(boid, 0);
            Vector2 flee = fleeFromPredators(boid, nearbyIndices);
            boid.applyForce(seekFood * 1.5f);
            boid.applyForce(flee * (3.0f * boid.genes.fearResponse));
            
        } else if (boid.type == 1) { // Carnivore
            Vector2 hunt = huntPrey(boid, nearbyIndices);
            boid.applyForce(hunt * (2.0f * boid.genes.aggression));
            
        } else if (boid.type == 2) { // Omnivore
            Vector2 seekFood = findNearestFood(boid, -1); // Any food
            Vector2 flee = fleeFromPredators(boid, nearbyIndices);
            boid.applyForce(seekFood * 1.2f);
            boid.applyForce(flee * (2.0f * boid.genes.fearResponse));
        } else if (boid.type == 3) { // Scavenger
            Vector2 seekMeat = findNearestFood(boid, 1); // Only meat
            boid.applyForce(seekMeat * 1.8f);
        }
        
        // Environmental forces
        Vector2 avoidObs = avoidObstacles(boid);
        boid.applyForce(avoidObs * 2.5f);
        
        Vector2 boundary = calculateBoundaryForce(boid);
        boid.applyForce(boundary * 2.0f);
        
        // Weather effects
        if (m_weather.windStrength > 2.0f) {
            Vector2 windForce = m_weather.getWindForce() * 0.1f;
            boid.applyForce(windForce);
        }
    }

    Vector2 seek(const Boid& boid, const Vector2& target) {
        Vector2 desired = target - boid.position;
        desired = VectorMath::normalize(desired) * boid.genes.maxSpeed;
        Vector2 steer = desired - boid.velocity;
        limitForce(steer, boid.maxForce);
        return steer;
    }

    void limitForce(Vector2& force, float maxForce) {
        float mag = force.magnitude();
        if (mag > maxForce) {
            force = VectorMath::normalize(force) * maxForce;
        }
    }

    Vector2 findNearestFood(const Boid& boid, int foodTypeFilter) {
        float closestDist = 1000000.0f;
        Vector2 target = boid.position;
        bool found = false;
        
        for (const auto& food : m_food) {
            if (food.consumed) continue;
            if (foodTypeFilter >= 0 && food.foodType != foodTypeFilter) continue;
            
            float dist = VectorMath::distance(boid.position, food.position);
            if (dist < closestDist && dist < 250.0f) {
                closestDist = dist;
                target = food.position;
                found = true;
            }
        }
        
        return found ? seek(boid, target) : Vector2(0, 0);
    }

    Vector2 fleeFromPredators(const Boid& boid, const std::vector<int>& nearbyIndices) {
        Vector2 steering(0, 0);
        int count = 0;
        
        for (int idx : nearbyIndices) {
            if (m_boids[idx].isDead || m_boids[idx].type != 1) continue;
            
            float dist = VectorMath::distance(boid.position, m_boids[idx].position);
            if (dist < 150.0f) {
                Vector2 diff = boid.position - m_boids[idx].position;
                if (dist > 0.0001f) diff /= (dist * dist);
                steering += diff;
                count++;
            }
        }
        
        if (count > 0) {
            steering /= static_cast<float>(count);
            steering = VectorMath::normalize(steering) * boid.genes.maxSpeed;
            steering -= boid.velocity;
            limitForce(steering, boid.maxForce * 2.0f);
        }
        
        return steering;
    }

    Vector2 huntPrey(const Boid& boid, const std::vector<int>& nearbyIndices) {
        float closestDist = 1000000.0f;
        Vector2 target = boid.position;
        bool found = false;
        
        for (int idx : nearbyIndices) {
            if (m_boids[idx].isDead || m_boids[idx].type == 1) continue;
            
            float dist = VectorMath::distance(boid.position, m_boids[idx].position);
            if (dist < closestDist && dist < 300.0f) {
                closestDist = dist;
                target = m_boids[idx].position;
                found = true;
            }
        }
        
        return found ? seek(boid, target) : Vector2(0, 0);
    }

    Vector2 avoidObstacles(const Boid& boid) {
        Vector2 steering(0, 0);
        
        for (const auto& obs : m_obstacles) {
            float dist = VectorMath::distance(boid.position, obs.position);
            float avoidRadius = obs.radius + 30.0f;
            
            if (dist < avoidRadius) {
                Vector2 diff = boid.position - obs.position;
                if (dist > 0.0001f) diff /= (dist * dist);
                steering += diff;
            }
        }
        
        if (steering.magnitude() > 0.0001f) {
            steering = VectorMath::normalize(steering) * boid.genes.maxSpeed;
            steering -= boid.velocity;
            limitForce(steering, boid.maxForce * 2.0f);
        }
        
        return steering;
    }

    Vector2 calculateBoundaryForce(const Boid& boid) {
        Vector2 steering(0, 0);
        const float margin = 50.0f;
        
        if (boid.position.x < margin) steering.x = boid.genes.maxSpeed;
        else if (boid.position.x > WORLD_WIDTH - margin) steering.x = -boid.genes.maxSpeed;
        
        if (boid.position.y < margin) steering.y = boid.genes.maxSpeed;
        else if (boid.position.y > WORLD_HEIGHT - margin) steering.y = -boid.genes.maxSpeed;
        
        return steering;
    }

    void wrapBoid(Boid& boid) {
        if (boid.position.x < 0) boid.position.x = WORLD_WIDTH;
        if (boid.position.x > WORLD_WIDTH) boid.position.x = 0;
        if (boid.position.y < 0) boid.position.y = WORLD_HEIGHT;
        if (boid.position.y > WORLD_HEIGHT) boid.position.y = 0;
    }

    void handleFoodConsumption() {
        for (auto& boid : m_boids) {
            if (boid.isDead) continue;
            
            for (auto& food : m_food) {
                if (food.consumed) continue;
                
                // Check if boid can eat this food
                bool canEat = false;
                if (boid.type == 0 && food.foodType == 0) canEat = true; // Herbivore eats plants
                else if (boid.type == 1 && food.foodType == 1) canEat = true; // Carnivore eats meat
                else if (boid.type == 2) canEat = true; // Omnivore eats anything
                else if (boid.type == 3 && food.foodType == 1) canEat = true; // Scavenger eats meat
                
                if (canEat) {
                    float dist = VectorMath::distance(boid.position, food.position);
                    if (dist < 10.0f) {
                        food.consumed = true;
                        boid.energy = std::min(100.0f, boid.energy + food.energy);
                    }
                }
            }
        }
    }

    void handlePredation() {
        for (auto& predator : m_boids) {
            if (predator.isDead || predator.type != 1) continue;
            
            for (auto& prey : m_boids) {
                if (prey.isDead || prey.type == 1) continue;
                
                float dist = VectorMath::distance(predator.position, prey.position);
                if (dist < 12.0f) {
                    prey.isDead = true;
                    predator.energy = std::min(100.0f, predator.energy + 60.0f);
                }
            }
        }
    }

    void handleReproduction() {
        if (m_boids.size() >= MAX_BOIDS - 10) return;
        
        for (size_t i = 0; i < m_boids.size(); ++i) {
            auto& boid = m_boids[i];
            if (boid.isDead || boid.isChild) continue;
            if (boid.energy < boid.genes.reproductionThreshold) continue;
            if (boid.reproductionCooldown > 0.0f) continue;
            
            // Check if near a nest
            bool nearNest = false;
            for (const auto& obs : m_obstacles) {
                if (obs.isNest) {
                    float dist = VectorMath::distance(boid.position, obs.position);
                    if (dist < obs.radius + 20.0f) {
                        nearNest = true;
                        break;
                    }
                }
            }
            
            if (!nearNest) continue;
            
            // Find a mate
            auto nearby = m_spatialGrid.query(boid.position, 50.0f);
            for (int idx : nearby) {
                if (idx == static_cast<int>(i)) continue;
                if (m_boids[idx].isDead || m_boids[idx].isChild) continue;
                if (m_boids[idx].type != boid.type) continue;
                if (m_boids[idx].energy < m_boids[idx].genes.reproductionThreshold) continue;
                
                // Reproduce!
                Vector2 childPos = (boid.position + m_boids[idx].position) * 0.5f;
                std::uniform_real_distribution<float> velDist(-30.0f, 30.0f);
                Vector2 childVel(velDist(m_rng), velDist(m_rng));
                
                int newGen = std::max(boid.generation, m_boids[idx].generation) + 1;
                m_generationMax = std::max(m_generationMax, newGen);
                
                Boid child(childPos, childVel, boid.color, boid.type, newGen);
                child.genes = boid.genes.mutate(m_rng);
                m_boids.push_back(child);
                
                // Cost of reproduction
                boid.energy -= 40.0f;
                m_boids[idx].energy -= 40.0f;
                boid.reproductionCooldown = 5.0f;
                m_boids[idx].reproductionCooldown = 5.0f;
                
                m_totalBirths++;
                break;
            }
        }
    }

    void printStats() {
        int herbivores = 0, carnivores = 0, omnivores = 0, scavengers = 0;
        float avgGeneration = 0.0f;
        
        for (const auto& boid : m_boids) {
            if (boid.type == 0) herbivores++;
            else if (boid.type == 1) carnivores++;
            else if (boid.type == 2) omnivores++;
            else scavengers++;
            avgGeneration += boid.generation;
        }
        
        if (!m_boids.empty()) {
            avgGeneration /= m_boids.size();
        }
        
        std::cout << "[T=" << static_cast<int>(m_globalTime) << "s] "
                << "H:" << herbivores << " C:" << carnivores 
                << " O:" << omnivores << " S:" << scavengers
                << " | Births:" << m_totalBirths << " Deaths:" << m_totalDeaths
                << " | AvgGen:" << avgGeneration << " MaxGen:" << m_generationMax
                << " | Weather: " << (m_weather.isRaining ? "Rain" : "Clear")
                << " Temp:" << static_cast<int>(m_weather.temperature) << "C\n";
    }
};
