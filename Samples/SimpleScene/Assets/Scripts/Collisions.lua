local ecs = require "ecs"

local function UpdateCollisions(it)
    local entities = {}

    for pos, hitbox, colstat, entity in ecs.each(it) do
        table.insert(entities, entity)
    end

    for i = 1, #entities do
        for j = i+1, #entities do
            local e1 = entities[i]
            local e2 = entities[j]

            local pos1 = ecs.get(e1, ecs.lookup("Position"))
            local pos2 = ecs.get(e2, ecs.lookup("Position"))
            local hitbox1 = ecs.get(e1, ecs.lookup("Hitbox"))
            local hitbox2 = ecs.get(e2, ecs.lookup("Hitbox"))
            local dx = pos1.x - pos2.x
            local dy = pos1.y - pos2.y
            local dz = pos1.z - pos2.z
            local dist_squared = dx*dx + dy*dy + dz*dz
            local sum_radius_squared = hitbox1.hitboxRadius + hitbox2.hitboxRadius
            sum_radius_squared = sum_radius_squared * sum_radius_squared

            if dist_squared < sum_radius_squared then
                local coll1 = ecs.get(e1, ecs.lookup("CollisionStatus"))
                if (not coll1.isActive) then
                    ecs.set(e1, ecs.lookup("CollisionStatus"), {1, 0})
                end
                local coll2 = ecs.get(e2, ecs.lookup("CollisionStatus"))
                if (not coll2.isActive) then
                    ecs.set(e2, ecs.lookup("CollisionStatus"), {1, 0})
                end
            else
                ecs.set(e1, ecs.lookup("CollisionStatus"), {0, 0})
                ecs.set(e2, ecs.lookup("CollisionStatus"), {0, 0})
            end
        end
    end
end

ecs.system(UpdateCollisions, "UpdateCollisions", ecs.OnUpdate, "Position, Hitbox, CollisionStatus")

-- isResolved не используется, но решил его оставить
local function ResolveCollisions(it)
    for v, colstat, e in ecs.each(it) do
        if colstat.isActive and not colstat.isResolved then
            v.y = -1 * v.y
        end
    end
end

ecs.system(ResolveCollisions, "ResolveCollisions", ecs.OnUpdate, "Velocity, CollisionStatus")

local function UpdateDestroyTimer(it)
    for timer, ent in ecs.each(it) do
        timer.timeLeft = timer.timeLeft - it.delta_time
        if timer.timeLeft <= 0 then
            ecs.delete(ent)
        end
    end
end

ecs.system(UpdateDestroyTimer, "UpdateDestroyTimer", ecs.OnUpdate, "DestroyTimer")

local function MovementRestrictionSystem(it)
    for pos, vel, restrict in ecs.each(it) do
        if restrict.restrictX then
            if pos.x <= restrict.minX and vel.x < 0 then
                vel.x = -vel.x
            elseif pos.x >= restrict.maxX and vel.x > 0 then
                vel.x = -vel.x
            end
        end
        
        if restrict.restrictY then
            if pos.y <= restrict.minY and vel.y < 0 then
                vel.y = -vel.y
            elseif pos.y >= restrict.maxY and vel.y > 0 then
                vel.y = -vel.y
            end
        end
        
        if restrict.restrictZ then
            if pos.z <= restrict.minZ and vel.z < 0 then
                vel.z = -vel.z
            elseif pos.z >= restrict.maxZ and vel.z > 0 then
                vel.z = -vel.z
            end
        end
    end
end

ecs.system(MovementRestrictionSystem, "MovementRestriction", ecs.OnUpdate, "Position, Velocity, MovementRestrictor")
