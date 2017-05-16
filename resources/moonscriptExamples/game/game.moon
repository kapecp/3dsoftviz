
import insert from table

export love = { graphics: {} }
lovekit = require "lovekit.init"

defines = [k for k in pairs lovekit]

for k in pairs lovekit.Sequence.default_scope
  insert defines, k

insert defines, "love"
insert defines, "CONTROLLER"
insert defines, "DISPATCHER"
insert defines, "GAME_CONFIG"
insert defines, "AUDIO"


export GAME_CONFIG = {
  scale: 3
  keys: {
    confirm: { "x", " " }
    cancel: "c"

    attack: { "x", " " }

    up: "up"
    down: "down"
    left: "left"
    right: "right"
  }
}

love.conf = (t) ->
  t.window.width = 300 * GAME_CONFIG.scale
  t.window.height = 200 * GAME_CONFIG.scale
  t.title = "Fish Deluxe - leafo's ludum dare 29"
  t.author = "leafo"

{graphics: g} = love

class ParalaxBg
  layers: 4
  speed: 1
  lazy sprite: -> imgfy "images/ground.png"

  new: =>
    @sprite\set_wrap "repeat", "clamp"

    @base_offsets = setmetatable {}, __index: (key) =>
      r = rand 0, 100
      @[key] = r
      r

  update: (dt) =>

  draw: (viewport, bounds) =>
    w,h = @sprite\width!, @sprite\height!
    vw = viewport.w

    for l=@layers,1,-1
      t = (@layers - (l - 1)) / @layers

      COLOR\push { t * 255, t * 255, t * 255 }

      scale = math.sqrt t
      real_w = w * scale

      times = math.floor(vw / real_w) + 2
      q = g.newQuad 0, 0, w * times, h, w, h

      g.push!
      g.translate viewport.x, bounds.h
      
      px = -(l - 1) / @layers + 1

      g.translate -((@base_offsets[l] + viewport.x) * px % real_w), (l - 1) * -10

      g.scale scale, scale

      @sprite\draw q, 0, -h
      g.pop!

      COLOR\pop!


{graphics: g} = love

import BubbleEmitter, BloodEmitter from require "particles"
import FadeAway from require "misc"

class Enemy extends Entity
  score: 1

  is_enemy: true
  w: 40
  h: 20

  ox: 0
  oy: 0

  slowing: 0
  threat: 0

  health: 1

  facing: "left"

  move_speed: 250
  move_time: 1.0

  new: (...) =>
    super ...
    @seqs = DrawList!
    @effects = EffectList!
    @seqs\add @make_ai!

  make_ai: =>
    error "implement ai for enemy #{@@__name}"

  move: (dir, fn) =>
    @seqs\add Sequence ->
      @slowing -= 1
      @move_accel = dir * @move_speed
      wait 0.15 * @move_time
      @move_accel = false
      wait 0.85 * @move_time
      @slowing += 1
      fn!

  charge: (dir, fn) =>
    @seqs\add Sequence ->
      amount = rand 500, 700
      @move_accel = dir * amount
      wait 0.3
      @move_accel = false

      @slowing += 1
      wait 1.0
      @slowing -= 1
      fn!

  attack: (thing, fn) =>
    dir = @vector_to(thing)\normalized!
    attack_force = 4000

    @seqs\add Sequence ->
      await (fn) ->
        @effects\add ShakeEffect 0.5, nil, nil, fn

      @mouth_emitter = BubbleEmitter @world, @mouth_box\center!
      @world.particles\add @mouth_emitter

      @move_accel = dir * attack_force
      wait 0.1
      @move_accel = false
      wait 0.1
      @slowing += 4
      wait 0.9
      @slowing -= 4

      fn!

  update: (dt, world) =>
    @world = world
    @seqs\update dt
    @effects\update dt

    ax, ay = 0,0

    if update_accel = @stun_accel or @move_accel
      ax += update_accel[1]
      ay += update_accel[2]

    if @slowing >= 0
      dampen_vector @vel, dt * 100 * (@slowing * 2 + 1)

    if not @stunned and ax != 0
      @facing = ax > 0 and "right" or "left"

    @vel\adjust ax * dt, ay * dt
    cx, cy = @fit_move @vel[1] * dt, @vel[2] * dt, world

    if cx
      @vel[1] = -@vel[1] / 2

    if cy
      @vel[2] = -@vel[2] / 2

    @anim\set_state @facing

    speed = @vel\len!
    @anim\update dt * (1 + speed / 100)

    @update_mouth!
    alive = @health > 0
    unless alive
      world.particles\add FadeAway @

    alive

  take_hit: (p, world) =>
    return if @stunned
    @threat = 3
    @just_hit = true
    power = 2500

    world.viewport\shake!
    world.particles\add BloodEmitter world, @center!
    @effects\add FlashEffect!

    @health -= 1

    if @health <= 0
      world.game.score += @score
      AUDIO\play "enemy_die"
    else
      AUDIO\play "hit2"

    @stunned = @seqs\add Sequence ->
      dir = p.mouth_box\vector_to(@)\normalized!
      @stun_accel = dir\normalized! * power
      wait 0.1
      @stun_accel = false

      @slowing += 1
      wait 0.5
      @slowing -= 1

      @stunned = false

  draw: =>
    @effects\before!
    @anim\draw @x - @ox, @y - @oy
    @effects\after!

    -- Box.outline @
    -- color = if @stunned
    --   {255,200,200}
    -- elseif @move_accel
    --   {20,20,20}
    -- else
    --   {255,255,255}

    -- super color

    -- COLOR\push {255, 0, 255, 128}
    -- @mouth_box\draw!
    -- COLOR\pop!

    -- if @slowing != 0
    --   g.print "Slowing #{@slowing}", @x, @y


  update_mouth: =>
    @mouth_box or= Box @x, @y, 10, 10
    @mouth_box.y = @y + (@h - @mouth_box.w) / 2
    @mouth_box.x = if @facing == "right"
      @x + @w - @mouth_box.w
    else
      @x

    if @mouth_emitter
      @mouth_emitter.x, @mouth_emitter.y = @mouth_box\center!

class Guppy extends Enemy
  score: 6

  w: 20
  h: 10

  ox: 14
  oy: 9

  health: 3

  lazy sprite: -> Spriter "images/enemy1.png", 50, 30

  new: (...) =>
    super ...
    with @sprite
      @anim = StateAnim "right", {
        left: \seq {
          0,1,2,3
        }, 0.4, true

        right: \seq {
          0,1,2,3
        }, 0.4
      }

  make_ai: =>
    Sequence ->
      if @stunned
        wait_until -> not @stunned

      toward_player = @vector_to @world.player
      dist_to_player = toward_player\len!

      action = if dist_to_player < 250
        pick_dist { charge: 1, move: 1 }
      else
        pick_dist { charge: 1, move: 3 }

      dir = if dist_to_player < 200 and math.random! < 0.5
        toward_player\normalized!
      else
        Vec2d.random!

      switch action
        when "charge"
          await @\charge, dir
        when "move"
          await @\move, dir

      wait rand 0.5, 0.6
      again!

class Shark extends Enemy
  lazy sprite: -> Spriter "images/enemy2.png", 50, 30
  score: 11

  w: 25
  h: 10

  health: 5

  new: (...) =>
    super ...

    with @sprite
      @anim = StateAnim "right", {
        left: \seq {
          ox: 6
          oy: 12

          0,1,2,3
        }, 0.4, true

        right: \seq {
          ox: 19
          oy: 12

          0,1,2,3
        }, 0.4
      }

  make_ai: =>
    Sequence ->
      if @stunned
        wait_until -> not @stunned

      toward_player = @vector_to @world.player
      dist_to_player = toward_player\len!
      left_of_player = toward_player[1] > 0

      move, attack = switch @threat
        when 0
          100, 1
        when 1
          4, 1
        else
          1, 2

      if @just_hit
        attack = 10*attack
        @just_hit = false

      switch pick_dist { :move, :attack }
        when "move"
          dir = if dist_to_player < 150
            pick_dist {
              left: 1 + (left_of_player and 1 or 0)
              right: 1 + (left_of_player and 0 or 1)
              player: 3
            }
          else
            pick_dist {
              left: 3
              right: 3
              player: 2
            }

          dir = switch dir
            when "left"
              Vec2d -1, 0
            when "right"
              Vec2d 1, 0
            when "player"
              (toward_player)\normalized!

          move, charge = switch @threat
            when 0
              3,1
            when 1
              1,1
            else
              1,3

          switch pick_dist { :move, :charge }
            when "charge"
              await @\charge, dir
            when "move"
              await @\move, dir
        when "attack"
          await @\attack, @world.player

      wait rand 0.4, 0.6
      again!

class Jelly extends Enemy
  score: 4

  w: 15
  h: 15

  ox: 13
  oy: 5

  health: 2

  lazy sprite: -> Spriter "images/enemy3.png", 40, 40

  new: (...) =>
    super ...

    with @sprite
      @anim = StateAnim "right", {
        left: \seq {
          0,1,2,3
        }, 0.4, true

        right: \seq {
          0,1,2,3
        }, 0.4
      }

  make_ai: =>
    Sequence ->
      if @stunned
        wait_until -> not @stunned

      toward_player = @vector_to @world.player
      dist_to_player = toward_player\len!
      left_of_player = toward_player[1] > 0

      dir = if dist_to_player < 150
        pick_dist {
          rand: 1
          player: 3
        }
      else
        pick_dist {
          rand: 2
          player: 1
        }

      dir = switch dir
        when "rand"
          Vec2d.random!
        when "player"
          (toward_player)\normalized!

      await @\move, dir
      wait rand 0.8, 1.1
      again!

class Snake extends Enemy
  lazy sprite: -> Spriter "images/enemy4.png", 50, 30
  score: 4

  w: 30
  h: 8

  ox: 8
  oy: 13

  health: 2

  new: (...) =>
    super ...

    with @sprite
      @anim = StateAnim "right", {
        left: \seq {
          0,1,2,3
        }, 0.4

        right: \seq {
          0,1,2,3
        }, 0.4, true
      }

  make_ai: =>
    Sequence ->
      if @stunned
        wait_until -> not @stunned

      toward_player = @vector_to @world.player
      dist_to_player = toward_player\len!
      left_of_player = toward_player[1] > 0

      dir = if dist_to_player < 200 and math.random! < 0.5
        toward_player\normalized!
      else
        Vec2d.random!

      await @\charge, dir
      wait rand 1.0, 1.2
      again!


class Sardine extends Enemy
  w: 8
  h: 8

  move_speed: 500
  move_time: 0.5

  health: 1

  lazy sprite: -> Spriter "images/enemy5.png", 16, 16

  new: (...) =>
    super ...

    with @sprite
      @anim = StateAnim "right", {
        left: \seq {
          ox: 0
          oy: 4

          0,1,2,3
        }, 0.4

        right: \seq {
          oy: 4
          ox: 4

          0,1,2,3
        }, 0.4, true
      }

  make_ai: =>
    Sequence ->
      if @stunned
        wait_until -> not @stunned

      toward_player = @vector_to @world.player
      dist_to_player = toward_player\len!

      dir = if dist_to_player < 200 and math.random! < 0.5
        toward_player\normalized!
      else
        Vec2d.random!

      await @\move, dir
      wait 0.2
      again!

{graphics: g} = love

import VList, HList, Label, Anchor from require "lovekit.ui"

class MessageBox
  padding: 5
  visible: true
  box_color: {0,0,0, 100}

  new: (@text) =>
    @alpha = 0
    @seq = Sequence ->
      tween @, 0.3, { alpha: 255 }
      @seq = nil

  draw: (viewport) =>
    left = viewport\left 10
    right = viewport\right 10
    bottom = viewport\bottom 10

    font = g.getFont!
    height = font\getHeight!
    width = right - left

    x = left
    y = bottom - height

    p = @padding

    COLOR\pusha @alpha
    g.push!
    g.translate x, y
    COLOR\push @box_color
    g.rectangle "fill", -p, -p, width + p * 2, height + p * 2
    COLOR\pop!
    g.print @text, 0,0
    g.pop!
    COLOR\pop!

  hide: =>
    return if @hiding or not @visible
    @hiding = true
    @seq = Sequence ->
      tween @, 0.2, { alpha: 0 }
      @hiding = false
      @visible = false

  update: (dt) =>
    @seq\update dt if @seq
    @visible

class HBar extends Box
  p: 0.5
  w: 100
  h: 12
  padding: 2

  draw: =>
    @outline!
    COLOR\push {255, 100, 100, 128}
    full_width = @w - @padding * 2
    g.rectangle "fill", @x + @padding, @y + @padding,
      full_width * @p, @h - @padding * 2
    COLOR\pop!


class Radar extends Box
  w: 50

  new: =>

  draw: =>
    Box.outline @

    g.push!
    g.translate @x, @y
    g.scale @w / @world.map_box.w, @h / @world.map_box.h
    g.setPointSize 4

    for e in *@world.entities
      continue unless e.alive
      continue unless e.center
      continue if e.is_misc

      pushed = if e.is_enemy
        COLOR\push 255,100,100, 128
        true
      elseif e.is_zone
        COLOR\push 100,255,100, 128
        true

      g.point e\center!

      COLOR\pop! if pushed

    g.pop!


  update: (dt, @world) =>
    r = @world.map_box.w / @world.map_box.h
    @h = @w / r
    true

class Hud
  margin: 10

  new: (world) =>
    @entities = DrawList!
    @display_score = world.game.score

    @health_bar = HBar!

    nice = (n) ->
      math.floor(n * 100) / 100

    @entities\add VList {
      x: @margin
      y: @margin

      HList {
        yalign: "center"
        Label "HP:"
        @health_bar

        Label -> "Score: #{math.floor @display_score}"
      }

      -- Label -> tostring love.timer.getFPS!
    }


    @radar = Anchor @margin, @margin, Radar!, "right", "top"
    @entities\add @radar

  clear_message_box: =>
    if @msg_box and @msg_box.visible
      @msg_box\hide!

  has_message_box: =>
    return false unless @msg_box
    return @msg_box.visible

  show_message_box: (mbox) =>
    @clear_message_box!

    @msg_box = mbox
    @entities\add mbox

  draw: (v) =>
    return unless @world
    g.push!
    g.translate v.x, v.y
    @entities\draw v
    g.pop!

  update: (dt, @world) =>
    p = world.player
    @health_bar.p = smooth_approach @health_bar.p, p.health / p.__class.health, dt
    @radar.x = world.viewport.w - @margin

    @display_score = smooth_approach @display_score, @world.game.score, dt * 2

    @entities\update dt, world

require "lovekit.all"
-- require "lovekit.reloader"

{graphics: g} = love

import Hud, MessageBox from require "hud"
import Player from require "player"
import Guppy, Shark, Jelly, Snake, Sardine from require "enemy"

import Ripple from require "shaders"

import ParalaxBg from require "background"

paused = false

import RevealLabel, Anchor from require "lovekit.ui"

local *

class Intro extends Sequence
  new: (@world, callback) =>
    @entities = DrawList!

    anchor = (label) ->
      cx, cy = @world.viewport\center!
      label.rate = 0.05
      Anchor cx, cy, label, "center", "center"

    intro = {
      "There's something foul\n      in the ocean\n(press 'X' to continue)"
      "The duty of a fish calls..."
      "Arrows move, 'X' attacks\nDouble tap to boost"
      "Go forth and rid the sea of evil"
    }

    super ->
      @world.hud_alpha = 0
      @world.seqs\add AUDIO\fade_music 0.75

      wait 2.0
      AUDIO\play "intro_explosion"
      @world.viewport\shake 2.0

      wait 2.0

      local current
      for msg in *intro
        if current
          @entities\remove current
          current = nil

        await (fn) ->
          current = anchor RevealLabel msg, 10, 10, fn
          @entities\add current

        wait_for_key unpack GAME_CONFIG.keys.confirm

      @world.seqs\add Sequence ->
        tween @world, 1.0, hud_alpha: 255

      callback!

  update: (dt) =>
    if CONTROLLER\is_down "cancel"
      dt *= 2

    @entities\update dt
    super dt

  draw: (...) =>
    @entities\draw ...

class Zone extends Box
  is_zone: true

  touching_player: 0

  activate: (world) =>
    error "replace me"

  is_ready: =>
    @touching_player > 0

  update: (dt, world) =>
    if @touching_player > 0
      @touching_player -= 1

    if @touching_player == 0 and @message_box
      @message_box\hide!
      @message_box = nil

    true

  take_hit: (player, world) =>
    @touching_player = 2

    if @is_ready! and @message and not @message_box
      @message_box = MessageBox @message
      world.hud\show_message_box @message_box

  draw: =>
    -- Box.outline @

class Transport extends Zone
  message: "Press 'C' to exit"

class RestZone extends Zone
  is_misc: true
  message: "Press 'C' to rest"
  new: (@world, ...) =>
    super ...

  is_ready: =>
    super! and @world.can_rest

class World
  gravity_mag: 130
  spawn_x: 300
  spawn_y: 300
  hud_alpha: 255
  overlay_alpha: 0

  new: (@game) =>
    @player = @game.player
    @player.vel[1] = 0
    @player.vel[2] = 0

    @viewport = EffectViewport scale: GAME_CONFIG.scale
    @entities = DrawList!
    @particles = DrawList!
    @seqs = DrawList!

    @set_player_pos!

    @entities\add @player
    @entities\add @exit if @exit
    @entities\add @rest if @rest

    @hud = Hud @

    @collide = UniformGrid!

    @shader = Ripple @viewport

  set_player_pos: (x=@spawn_x, y=@spawn_y) =>
    @player.x, @player.y = x, y
    @viewport\center_on @player

  on_show: =>
    return if @game.show_intro
    @start_music!

  start_music: =>
    unless AUDIO.current_music == "main"
      AUDIO\play_music "main"

  mousepressed: (x,y) =>
    x, y = @viewport\unproject x, y
    for e in *@entities
      continue unless e.is_enemy
      e.facing = e.facing == "left" and "right" or "left"

  on_key: (key) =>
    if key == "p"
      paused = not paused

    if CONTROLLER\is_down "cancel"
      if @exit\is_ready!
        @exit\activate @

      if @rest and @rest\is_ready!
        @rest\activate @

  draw: =>
    @shader\render ->
      @viewport\apply!

      if @map
        @map\draw @viewport

      COLOR\pusha 64
      show_grid @viewport, 20, 20
      COLOR\pop!

      @entities\draw!
      @particles\draw!

      if @overlay_alpha and @overlay_alpha > 0
        @viewport\apply!
        COLOR\push {0,0,0, @overlay_alpha}
        Box.draw @viewport
        COLOR\pop!
        @viewport\pop!

      @viewport\pop!

    @viewport\apply!

    COLOR\pusha @hud_alpha
    @hud\draw @viewport
    COLOR\pop!

    @viewport\pop!

  gravity: (vec, dt) =>
    do return
    return unless @gravity_pull
    vec\adjust unpack @gravity_pull * dt

  collides: (thing) =>
    if @map
      @map\collides(thing) or not @map_box\contains_box thing

  update: (dt) =>
    return if paused

    @_t or= 0
    @_t += dt
    @gravity_pull = Vec2d.from_angle(90 + math.sin(@_t * 2) * 7) * @gravity_mag

    @hud\update dt, @

    @viewport\update dt
    @viewport\center_on @player, @map_box, dt

    @entities\update dt, @
    @particles\update dt, @
    @seqs\update dt, @

    @collide\clear!

    for e in *@entities
      continue unless e.alive
      continue unless e.w -- is boxy
      @collide\add e

    for e in *@collide\get_touching @player
      continue unless e.take_hit
      continue if e.stunned
      @player\take_hit e, @

-- wide open
class OceanMap extends Box
  x: 0
  y: 0

  w: 1000
  h: 300

  new: =>
    @bg = ParalaxBg!

  draw: (viewport) =>
    @bg\draw viewport, @

  update: (dt) =>

  collides: (thing) =>
    not @contains_box thing


import
  SardineSpawner
  JellySpawner
  GuppySpawner
  SnakeSpawner
  SharkSpawner
  from require "spawners"

class Ocean extends World
  levels: {
    => -- 1
      SardineSpawner(@)\spawn 5
      SardineSpawner(@)\spawn 8

    => -- 2
      SardineSpawner(@)\spawn 8
      JellySpawner(@)\spawn 3
      JellySpawner(@)\spawn 3

    => -- 3
      JellySpawner(@)\spawn 3
      GuppySpawner(@)\spawn 2
      SnakeSpawner(@)\spawn 2

    => -- 4
      SharkSpawner(@)\spawn 1

      GuppySpawner(@)\spawn 1
      GuppySpawner(@)\spawn 1
      GuppySpawner(@)\spawn 1

      SnakeSpawner(@)\spawn 2
      SnakeSpawner(@)\spawn 2


    => -- 5
      SharkSpawner(@)\spawn 2
      SharkSpawner(@)\spawn 2

      GuppySpawner(@)\spawn 3
      GuppySpawner(@)\spawn 3

      SardineSpawner(@)\spawn 5
      SardineSpawner(@)\spawn 5
  }

  new: (...) =>
    @map = OceanMap @
    @map_box = @map

    @exit = Transport 0, @map_box.h - 100, 100, 100
    @exit.message = "Press 'C' to return to home"
    @exit.activate = ->
      @entities\remove @player
      home = Home @game
      home.can_rest = not @has_enemies!

      DISPATCHER\replace home
      home\set_player_pos home.exit\center!

    @spawn_x, @spawn_y = @exit\center!

    super ...

    if level = @levels[@game.current_level]
      level @

  on_show: (...) =>
    super ...
    if @game\beat_game!
      import GameOver from require "screens"
      DISPATCHER\replace GameOver @game

  has_enemies: =>
    has_enemies = false
    for e in *@entities
      if e.alive and e.is_enemy
        has_enemies = true
        break

    has_enemies

  update: (dt) =>
    has_enemies = @has_enemies!

    if not has_enemies and not @return_mb
      @return_mb = MessageBox "The ocean is calm, return home"
      @hud\show_message_box @return_mb

    super dt


class Home extends World
  can_rest: false

  new: (...) =>
    @map = TileMap.from_tiled "maps/home", {
      object: (o) ->
        switch o.name
          when "spawn"
            @spawn_x = o.x
            @spawn_y = o.y
          when "rest"
            @rest = RestZone @, o.x, o.y, o.width, o.height
            @rest.activate = @\do_rest
          when "exit"
            @exit = Transport o.x, o.y, o.width, o.height
            @exit.message = "Press 'C' to enter the sea"
            @exit.activate = ->
              @entities\remove @player
              DISPATCHER\replace Ocean @game
    }

    @map_box = @map\to_box!

    super ...

    if @game.show_intro
      @player.locked = true
      @entities\add Intro @, ->
        @game.show_intro = false
        @start_music!
        @player.locked = false

  do_rest: =>
    @can_rest = false

    @seqs\add Sequence ->
      @player.locked = true

      @hud\clear_message_box!
      tween @, 1.0, overlay_alpha: 255
      @player.health = @player.__class.health
      AUDIO\play "recover"
      wait 1.0
      tween @, 1.0, overlay_alpha: 0

      local current
      await (fn) ->
        label = with RevealLabel "The feral fish grow stronger", 10, 10, fn
          .rate = 0.05
        cx, cy = @viewport\center!
        current = Anchor cx, cy, label, "center", "center"

        @entities\add current

      wait_for_key unpack GAME_CONFIG.keys.confirm
      @entities\remove current
      @player.locked = false
      @game.current_level += 1

class Game
  current_level: 1
  show_intro: true

  @start: =>
    game = Game!
    Home game

  beat_game: =>
    not Ocean.levels[@current_level]

  new: =>
    @score = 0
    @player = Player 0, 0

load_font = (img, chars)->
  font_image = imgfy img
  g.newImageFont font_image.tex, chars

love.load = ->
  fonts = {
    default: load_font "images/font1.png", [[ ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~!"#$%&'()*+,-./0123456789:;<=>?]]
  }

  g.setFont fonts.default

  g.setBackgroundColor 12,14, 15

  import Title from require "screens"

  export AUDIO = Audio "sounds"
  AUDIO\preload {
    "bump_wall"
    "charge"
    "enemy_die"
    "hit1"
    "hit2"
    "recover"
    "start"
    "player_die"
    "boost"
    "intro_explosion"
  }

  -- AUDIO.play_music = =>
  --   @music = setmetatable {}, __index: -> ->

  export CONTROLLER = Controller GAME_CONFIG.keys
  export DISPATCHER = Dispatcher Title Game\start!

  DISPATCHER.default_transition = FadeTransition
  DISPATCHER\bind love


{graphics: g} = love

class FadeAway extends Sequence
  new: (@thing) =>
    super ->
      @alpha = 255
      @scale = 1
      tween @, 0.5, {
        alpha: 0
        scale: 3
      }

  draw: =>
    cx, cy = @thing\center!

    g.push!
    g.translate cx, cy
    COLOR\pusha @alpha
    g.scale @scale, @scale

    g.translate -cx, -cy
    @thing\draw!

    COLOR\pop!
    g.pop!
  

{ graphics: g } = love

class Blood extends PixelParticle
  size: 50
  life: 2

  new: (@x, @y) =>
    @size = rand 20,50
    @vel = Vec2d.random! * rand(20, 40)
    @accel = Vec2d 0, 0

  draw: =>
    t = ad_curve (1 - @life / @@life), 0, 0.05, 0.8

    COLOR\push {80,10,10, t * 128}
    super!
    COLOR\pop!

  update: (dt) =>
    dampen_vector @vel, dt * 10
    super dt

class Bubble extends PixelParticle
  size: 3
  life: 10.0

  new: (@x, @y) =>
    @size = rand 2,5
    @vel = Vec2d(0, -rand(15,30))\random_heading!
    @offset = rand(0, 10)
    @accel = Vec2d 0, -rand(35,45)

  update: (dt) =>
    @accel[1] = math.sin(@offset + @life * 8) * 100
    super dt

class BubbleEmitter extends Emitter
  spread_x: 8
  spread_y: 8

  count: 30
  duration: 0.5
  make_particle: (x, y) =>
    x += rand -@spread_x, @spread_x
    y += rand -@spread_y, @spread_y

    Bubble x,y


class BloodEmitter extends Emitter
  count: 4
  duration: 0.5
  make_particle: (x, y) =>
    Blood x,y


{graphics: g} = love

import BubbleEmitter, BloodEmitter from require "particles"
import FadeAway from require "misc"

class BoostEmitter extends BubbleEmitter
  count: 5
  duration: 0.5

  spread_x: 5
  spread_y: 1


class Player extends Entity
  is_player: true
  speed: 200
  max_speed: 100
  facing: "right"
  locked: false

  health: 100

  lazy sprite: -> Spriter "images/player.png", 50, 30

  w: 30
  h: 10

  ox: 10
  oy: 8

  new: (...) =>
    super ...
    @seqs = DrawList!
    @effects = EffectList!

    with @sprite
      @anim = StateAnim "right", {
        left: \seq {
          0,1,2,3
        }, 0.4, true

        right: \seq {
          0,1,2,3
        }, 0.4

        left_attacking: \seq {
          4,5,6,7
        }, 0.4, true

        right_attacking: \seq {
          4,5,6,7
        }, 0.4
      }

    @update_mouth!

  looking_at: (viewport) =>
    cx, cy = @center!
    switch @facing
      when "left"
        cx -= viewport.w / 10
      when "right"
        cx += viewport.w / 10

    cx, cy

  boost: (world, dir) =>
    return if @locked
    return if @stunned
    return if @boosting

    boost_power = 1000
    AUDIO\play "boost"
    @boosting = @seqs\add Sequence ->
      @boost_emitter = world.particles\add BoostEmitter world, @tail_center!
      @boost_accel = dir * boost_power

      wait 0.1
      @boost_accel = false
      wait 0.3
      @boosting = false

  attack: (world) =>
    return if @locked
    return if @stunned
    return if @attacking or @attacking_cooloff
    attack_force = 2500

    @mouth_emitter = BubbleEmitter world, @mouth_box\center!
    world.particles\add @mouth_emitter
    AUDIO\play "charge"

    @attacking = @seqs\add Sequence ->
      @vel[1] = 0

      force = attack_force
      force = -force if @facing == "left"

      @attack_accel = Vec2d force, 0

      wait 0.2
      @attack_accel = Vec2d -force/2, 0
      wait 0.2

      @attacking = false
      @attacking_cooloff = true
      wait 0.5
      @attacking_cooloff = false

  tail_center: =>
    x,y = @center!
    if @facing == "left"
      x += @w/2

    if @facing == "right"
      x -= @w/2

    x,y

  update_mouth: =>
    @mouth_box or= Box @x, @y, 10, 10
    @mouth_box.y = @y + (@h - @mouth_box.w) / 2
    @mouth_box.x = if @facing == "right"
      @x + @w - @mouth_box.w
    else
      @x

    if @mouth_emitter
      @mouth_emitter.x, @mouth_emitter.y = @mouth_box\center!

    if @boost_emitter
      @boost_emitter.x, @boost_emitter.y = @tail_center!

  update: (dt, world) =>
    @strafing = CONTROLLER\is_down "attack"
    if CONTROLLER\tapped "attack"
      @attack world

    @seqs\update dt, world
    @effects\update dt, world

    @accel = CONTROLLER\movement_vector @speed
    @accel = Vec2d! if @locked

    decel = @speed / 100

    dtu, dtd, dtl, dtr = CONTROLLER\double_tapped "up", "down", "left", "right"

    if dtu or dtd or dtl or dtr
      xx = dtl and -1 or (dtr and 1) or 0
      yy = dtu and -1 or (dtd and 1) or 0
      @boost world, Vec2d xx, yy

    if @attacking
      @accel[1] = @attack_accel[1]
    elseif @stunned
      @accel[1], @accel[2] = unpack @stun_accel
    else
      if @accel[1] != 0 and not @strafing
        @facing = @accel[1] > 0 and "right" or "left"

      if @boost_accel
        @accel[1] += @boost_accel[1]
        @accel[2] += @boost_accel[2]

    if @accel[1] == 0
      -- not moving in x, shrink it
      @vel[1] = dampen @vel[1], decel

    if @accel[2] == 0
      @vel[2] = dampen @vel[2], decel

    -- apply the ocean
    if @accel\is_zero!
      world\gravity @vel, dt

    @vel\adjust unpack @accel * dt
    unless @attacking or @boosting
      @vel\cap @max_speed

    cx, cy = @fit_move @vel[1] * dt, @vel[2] * dt, world

    if (cx or cy) and not @hit_audio
      @hit_audio = @seqs\add Sequence ->
        AUDIO\play "bump_wall"
        wait 0.2
        @hit_audio = nil

    if cx
      @vel[1] = 0

    if cy
      @vel[2] = -@vel[2] / 2

    @update_mouth!
    state = @facing
    if @attacking
      state = "#{state}_attacking"

    @anim\set_state state
    speed = @vel\len!

    @anim\update dt * (1 + speed / 100)
    alive = @health > 0

    unless alive
      world.particles\add BloodEmitter world, @center!
      world.particles\add FadeAway @
      AUDIO\play "player_die"

      world.particles\add Sequence ->
        import GameOver from require "screens"
        wait 1.0
        DISPATCHER\replace GameOver world.game

    alive

  draw: =>
    @effects\before!
    @anim\draw @x - @ox, @y - @oy
    @effects\after!

    --Box.outline @
    -- color = if @attacking
    --   {255,0, 0, 128}
    -- elseif @stunned
    --   {255,0, 255, 128}
    -- else
    --   {0,255, 0, 128}

    -- COLOR\push color
    -- @mouth_box\outline!
    -- COLOR\pop!

  take_hit: (enemy, world) =>
    if enemy.is_zone
      enemy\take_hit @, world
      return

    if @attacking
      if @mouth_box\touches_box enemy
        enemy\take_hit @, world
      return

    return if @stunned
    knockback = 2000
    world.viewport\shake nil, nil, 2
    @effects\add FlashEffect!
    AUDIO\play "hit1"

    @stunned = @seqs\add Sequence ->
      @health = math.max 0, @health - 15
      dir = enemy\vector_to(@)\normalized!

      @vel[1] = @vel[1] / 2
      @vel[2] = @vel[2] / 2
      @stun_accel = dir * knockback
      wait 0.075
      @stun_accel = Vec2d!
      wait 0.3
      @stunned = false

  __tostring: =>  "<Player #{Box.__tostring @}>"



import Ripple from require "shaders"
import Anchor, RevealLabel from require "lovekit.ui"

class Transition extends FadeTransition
  time: 1.5
  color: {10, 10, 10}

class Title
  lazy background: -> imgfy "images/title.png"

  new: (@start) =>
    @viewport = Viewport scale: GAME_CONFIG.scale
    @shader = Ripple @viewport
    @entities = DrawList!

    l = RevealLabel "Press 'X' to begin"
    l.rate = 0.1

    cx, cy = @viewport\center!
    @entities\add Anchor cx, cy + 70, l, "center", "center"

  on_show: =>
    unless AUDIO.current_music == "title"
      AUDIO\play_music "title"
      AUDIO.music\setVolume 0.25

  draw: =>
    @shader\render ->
      @viewport\apply!
      @background\draw 0, 0
      @entities\draw @viewport
      @viewport\pop!

  update: (dt) =>
    @entities\update dt

  on_key: =>
    if CONTROLLER\is_down "confirm"
      AUDIO\play "start"
      DISPATCHER\replace @start, Transition


class GameOver
  new: (game) =>
    @viewport = Viewport scale: GAME_CONFIG.scale
    @shader = Ripple @viewport
    @entities = DrawList!

    msg = if game\beat_game!
      "You're conquered the ocean"
    else
      "You have become fish fodder"

    l = RevealLabel "#{msg}\nPress 'X' to return to title\nThanks for playing!\nYour score: #{game.score}"

    cx, cy = @viewport\center!
    @entities\add Anchor cx, cy - 20, l, "center", "center"

  draw: =>
    @shader\render ->
      @viewport\apply!
      @entities\draw @viewport
      @viewport\pop!

  update: (dt) =>
    @entities\update dt

  on_show: =>
    if AUDIO.music
      AUDIO.music\stop!

  on_key: =>
    import Game from require "main"

    if CONTROLLER\is_down "confirm"
      DISPATCHER\replace Title Game\start!



{graphics: g} = love

class Shader
  shader: => error "override me"

  new: (@viewport) =>
    @canvas = g.newCanvas!
    @canvas\setFilter "nearest", "nearest"
    @canvas\setWrap "repeat", "repeat"

    @shader = g.newShader @shader!

  send: =>

  render: (fn) =>
    old_canvas = g.getCanvas!

    g.setCanvas @canvas
    @canvas\clear 0,0,0,0

    fn!

    if old_canvas
      g.setCanvas old_canvas
    else
      g.setCanvas!

    g.setBlendMode "premultiplied"
    g.setShader @shader unless @disabled
    @send!
    g.draw @canvas, 0,0
    g.setShader!
    g.setBlendMode "alpha"

class Ripple extends Shader
  send: =>
    @shader\send "pixel_ratio_x", 1/@viewport.w
    -- @shader\send "pixel_ratio_y", 1/@viewport.h
    @shader\send "time", love.timer.getTime!

  shader: -> [[
    #define M_PI 3.1415926535897932384626433832795

    extern number time;
    extern number pixel_ratio_x;

    vec4 desaturate(vec4 color, number amount) {
      number gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
      return mix(color, vec4(gray), amount);
    }

    vec4 effect(vec4 color, Image texture, vec2 texture_coords, vec2 screen_coords) {
      number x = texture_coords.x
        + sin(texture_coords.x * 10 + time) * (pixel_ratio_x * 2)
        + sin(time + texture_coords.y * M_PI * 5) * (pixel_ratio_x / 2);

      number y = texture_coords.y;

      vec4 c = Texel(texture, vec2(x, y));

      number dist = clamp(length(texture_coords - vec2(0.5, 0.5)) * 2, 0, 1) / 5;

      c = desaturate(c, pow(dist, .333)) * clamp(1 - dist*3, 0, 1);

      c.b *= (1 + dist);
      c.r *= (1 - dist);
      c.g *= (1 - dist);
      return c * color;
    }
  ]]



import Guppy, Shark, Jelly, Snake, Sardine from require "enemy"

class Spawner extends Box
  radius: 40

  new: (@world) =>
    @area = world.map_box\shrink @radius * 1.5

  position: =>
    with spawn_area = Box 0,0, @radius, @radius
      spawn_area\move_center @area\random_point!

  spawn: (num=1) =>
    spawn_area = @position!
    while spawn_area\touches_box @world.exit
      spawn_area = @position!

    for i=1,num
      @world.entities\add @create_enemy spawn_area\random_point!

  create_enemy: (x, y) =>
    error "override create_enemy #{@@.__name}"

class SardineSpawner extends Spawner
  create_enemy: (x,y) => Sardine x, y

class JellySpawner extends Spawner
  create_enemy: (x,y) => Jelly x, y

class GuppySpawner extends Spawner
  create_enemy: (x,y) => Snake x, y

class SnakeSpawner extends Spawner
  create_enemy: (x,y) => Snake x, y

class SharkSpawner extends Spawner
  create_enemy: (x,y) => Shark x, y





{ :Spawner, :SardineSpawner, :JellySpawner, :GuppySpawner, :SnakeSpawner,
  :SharkSpawner 
  :Ripple 
  :Title, :GameOver 
  :Player 
  :BubbleEmitter
  :BloodEmitter
  :FadeAway 
  :Game, :Home, :Ocean 
  :Hud, :MessageBox 
  :Enemy, :Guppy, :Shark, :Jelly, :Snake, :Sardine 
  :ParalaxBg 
  whitelist_globals: {
    ".": defines
  }
}
