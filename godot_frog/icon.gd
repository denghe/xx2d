extends Sprite

var draging = false
var draging_offset = Vector2()
onready var parent = $"../"
onready var tree = get_tree()

func _enter_tree():
	add_child(Node.new())
	get_parent().remove_child(self)

func _ready():
	set_process_unhandled_input(true)

func _unhandled_input(e):
	if e is InputEventMouseButton and e.button_index == BUTTON_LEFT:
		if e.pressed:
			if get_rect().has_point(to_local(e.position)):
				draging = true
				draging_offset = position - e.position
				parent.move_child(self, parent.get_child_count() - 1)
				tree.set_input_as_handled()
		else:
			if draging:
				draging = false
				tree.set_input_as_handled()
				
	elif e is InputEventMouse and draging:
		position = e.position + draging_offset
