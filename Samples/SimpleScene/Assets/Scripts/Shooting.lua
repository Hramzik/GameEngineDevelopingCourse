local ecs = require "ecs"

-- local function CheckCollision(pos1, hitboxSize1, pos2, hitboxSize2)
--     return pos1.x - hitboxSize1.x < pos2.x + hitboxSize2.x and
--            pos2.x - hitboxSize2.x < pos1.x + hitboxSize1.x and
--            pos1.y - hitboxSize1.y < pos2.y + hitboxSize2.y and
--            pos2.y - hitboxSize2.y < pos1.y + hitboxSize1.y and
--            pos1.z - hitboxSize1.z < pos2.z + hitboxSize2.z and
--            pos2.z - hitboxSize2.z < pos1.z + hitboxSize1.z
-- end

local function CheckCollision(pos1, _, pos2, _)
    local dx = pos1.x - pos2.x
    local dy = pos1.y - pos2.y
    local dz = pos1.z - pos2.z
    return dx * dx + dy * dy + dz * dz < 16
end

local function DestroyTimerSystem(it)
    for timer, ent in ecs.each(it, "DestroyTimer") do
        timer.time = timer.time - it.delta_time
        if timer.time <= 0 then
            ecs.delete(ent)
        end
    end
end

-- local function BulletHitSystem(it)
--     for pos1, hitbox1, bulletEntity in ecs.each(it) do
--         for pos2, hitbox2, targetEntity in ecs.each(it) do
--             if CheckCollision(pos1, hitbox1, pos2, hitbox2) then
--                 -- health.value = health.value - bullet.damage
--                 ecs.delete(bulletEntity)
--             end
--         end
--     end
-- end

-- local function BulletHitSystem(it)
--     for pos1, hitbox1, bulletEntity in ecs.each(it) do
--         if ecs.has(bulletEntity, ecs.lookup("Bullet")) then
--             for pos2, hitbox2, targetEntity in ecs.each(it) do
--                 if ecs.has(targetEntity, ecs.lookup("Health")) then
--                     if CheckCollision(pos1, hitbox1, pos2, hitbox2) then
--                         ecs.delete(bulletEntity)
--                         ecs.delete(targetEntity)
--                     end
--                 end
--             end
--         end
--     end
-- end

local bullets = {}
local targets = {}

local function CollectTargetsSystem(it)
    targets = {}
    for pos, hitbox, health, entity in ecs.each(it) do
        table.insert(targets, {pos = pos, hitbox = hitbox, entity = entity, health = health})
    end
end

local function CollectBulletsSystem(it)
    bullets = {}
    for pos, hitbox, bullet, entity in ecs.each(it) do
        table.insert(bullets, {pos = pos, hitbox = hitbox, entity = entity, bullet = bullet, destroyed = false})
    end
end

ecs.system(CollectTargetsSystem, "CollectTargetsSystem", ecs.OnUpdate, "Position, HitboxSize, Health")
ecs.system(CollectBulletsSystem, "CollectBulletsSystem", ecs.OnUpdate, "Position, HitboxSize, Bullet")

local function BulletHitSystem(_)
    for _, bullet in ipairs(bullets) do
        for _, target in ipairs(targets) do
            if CheckCollision(bullet.pos, bullet.hitbox, target.pos, target.hitbox) and not bullet.destroyed then
                bullet.destroyed = true
                ecs.delete(bullet.entity)
                break
            end
        end
    end
end

ecs.system(BulletHitSystem, "BulletHitSystem", ecs.OnUpdate)

local function HealthSystem(it)
    for health, ent in ecs.each(it, "Health") do
        if health.value <= 0 then
            ecs.delete(ent)
        end
    end
end

ecs.system(DestroyTimerSystem, "DestroyTimerSystem", ecs.OnUpdate, "DestroyTimer")
ecs.system(HealthSystem, "HealthSystem", ecs.OnUpdate, "Health")

