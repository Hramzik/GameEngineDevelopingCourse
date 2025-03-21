local ecs = require "ecs"

local function CheckCollision(pos1, hitboxSize1, pos2, hitboxSize2)
    return pos1.x - hitboxSize1.x < pos2.x + hitboxSize2.x and
           pos2.x - hitboxSize2.x < pos1.x + hitboxSize1.x and
           pos1.y - hitboxSize1.y < pos2.y + hitboxSize2.y and
           pos2.y - hitboxSize2.y < pos1.y + hitboxSize1.y and
           pos1.z - hitboxSize1.z < pos2.z + hitboxSize2.z and
           pos2.z - hitboxSize2.z < pos1.z + hitboxSize1.z
end

local function DestroyTimerSystem(it)
    for timer, ent in ecs.each(it, "DestroyTimer") do
        timer.time = timer.time - it.delta_time
        if timer.time <= 0 then
            ecs.delete(ent)
        end
    end
end

ecs.system(DestroyTimerSystem, "DestroyTimerSystem", ecs.PostUpdate, "DestroyTimer")

local bullets = {}

local function CollectBulletsSystem(it)
    bullets = {}
    for pos, hitbox, bullet, entity in ecs.each(it) do
        table.insert(bullets, {pos = pos, hitbox = hitbox, entity = entity, bullet_comp = bullet, destroyed = false})
    end
end

ecs.system(CollectBulletsSystem, "CollectBulletsSystem", ecs.PreFrame, "Position, HitboxSize, Bullet")

local function BulletHitSystem(it)
    for i, bullet in ipairs(bullets) do
        for target_pos, target_hitbox, target_health, target_entity in ecs.each(it) do
            if CheckCollision(bullet.pos, bullet.hitbox, target_pos, target_hitbox) and not bullet.destroyed then
                bullet.destroyed = true
                ecs.delete(bullet.entity)
                -- ecs.set(bullet.entity, ecs.lookup("DestroyTimer"), {0.01})
                ecs.set(target_entity, ecs.lookup("Health"), {target_health.value - bullet.bullet_comp.damage})
                break
            end
        end
    end
end

ecs.system(BulletHitSystem, "BulletHitSystem", ecs.OnUpdate, "Position, HitboxSize, Health")

local function HealthSystem(it)
    for health, ent in ecs.each(it, "Health") do
        if health.value <= 0 then
            ecs.delete(ent)
        end
    end
end

ecs.system(HealthSystem, "HealthSystem", ecs.PostUpdate, "Health")

