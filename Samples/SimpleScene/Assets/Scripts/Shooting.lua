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
    return dx * dx + dy * dy + dz * dz < 4
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

local targets = {}

local function CollectTargetsSystem(it)
    targets = {}
    for pos, hitbox, health, entity in ecs.each(it) do
        table.insert(targets, {pos = pos, hitbox = hitbox, entity = entity})
    end
end

ecs.system(CollectTargetsSystem, "CollectTargetsSystem", ecs.OnUpdate, "Position, HitboxSize, Health")

local function BulletHitSystem(it)
    for pos, bulletHitbox, bullet, bulletEntity in ecs.each(it) do
        for _, target in ipairs(targets) do
            if CheckCollision(pos, bulletHitbox, target.pos, target.hitbox) then
                ecs.delete(bulletEntity)
                ecs.delete(target.entity)
                break
            end
        end
    end
end

ecs.system(BulletHitSystem, "BulletHitSystem", ecs.OnUpdate, "Position, HitboxSize, Bullet")

local function HealthSystem(it)
    for health, ent in ecs.each(it, "Health") do
        if health.value <= 0 then
            health.value = 0
        end
    end
end

ecs.system(DestroyTimerSystem, "DestroyTimerSystem", ecs.OnUpdate, "DestroyTimer")
ecs.system(HealthSystem, "HealthSystem", ecs.OnUpdate, "Health")

