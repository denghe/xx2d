extends Node2D

onready var frog = $frog

func _unhandled_key_input(event):
	if event.is_action_pressed("ui_select"):
		frog.play("jump")

func _on_frog_animation_finished():
	frog.play("idle")


func _on_Button2_pressed():
	$Dialog.popup()
