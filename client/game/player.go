components {
  id: "player"
  component: "/client/game/player.script"
}
components {
  id: "engine"
  component: "/client/assets/particlefx/engine.particlefx"
  rotation {
    z: 1.0
    w: 6.123234E-17
  }
}
embedded_components {
  id: "sprite"
  type: "sprite"
  data: "default_animation: \"player\"\n"
  "material: \"/builtins/materials/sprite.material\"\n"
  "textures {\n"
  "  sampler: \"texture_sampler\"\n"
  "  texture: \"/client/assets/game.atlas\"\n"
  "}\n"
  ""
  rotation {
    z: 0.70710677
    w: 0.70710677
  }
}
embedded_components {
  id: "collisionobject"
  type: "collisionobject"
  data: "type: COLLISION_OBJECT_TYPE_KINEMATIC\n"
  "mass: 0.0\n"
  "friction: 0.1\n"
  "restitution: 0.5\n"
  "group: \"player\"\n"
  "mask: \"bullet\"\n"
  "mask: \"enemy\"\n"
  "mask: \"player\"\n"
  "embedded_collision_shape {\n"
  "  shapes {\n"
  "    shape_type: TYPE_SPHERE\n"
  "    position {\n"
  "    }\n"
  "    rotation {\n"
  "    }\n"
  "    index: 0\n"
  "    count: 1\n"
  "  }\n"
  "  data: 12.5\n"
  "}\n"
  ""
}
embedded_components {
  id: "wallcollider"
  type: "collisionobject"
  data: "type: COLLISION_OBJECT_TYPE_KINEMATIC\n"
  "mass: 0.0\n"
  "friction: 0.1\n"
  "restitution: 0.5\n"
  "group: \"wall_collider\"\n"
  "mask: \"wall\"\n"
  "embedded_collision_shape {\n"
  "  shapes {\n"
  "    shape_type: TYPE_SPHERE\n"
  "    position {\n"
  "      y: -5.0\n"
  "    }\n"
  "    rotation {\n"
  "    }\n"
  "    index: 0\n"
  "    count: 1\n"
  "  }\n"
  "  shapes {\n"
  "    shape_type: TYPE_SPHERE\n"
  "    position {\n"
  "      y: 13.0\n"
  "    }\n"
  "    rotation {\n"
  "    }\n"
  "    index: 1\n"
  "    count: 1\n"
  "  }\n"
  "  data: 15.0\n"
  "  data: 8.0\n"
  "}\n"
  ""
}
embedded_components {
  id: "spark_factory"
  type: "factory"
  data: "prototype: \"/client/game/spark.go\"\n"
  ""
}
