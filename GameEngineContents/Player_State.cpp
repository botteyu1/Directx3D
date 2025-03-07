#include "PreCompile.h"
#include "Player.h"
#include "BaseActor.h"
#include "BoneSocketCollision.h"
void Player::Player_State()
{
	{
		CreateStateParameter NewPara;

		// 한번만 실행되는 겁니다.
		NewPara.Init = [=](class GameEngineState* _Parent)
			{

			};


		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Shield_Col->Off();
				Top_Shield_Col->Off();
				Attack_Col->Off(); 
				Body_Col->On();
				Weapon_Actor->Getweapon()->On();
				Sword.ResetRecord();
				Hit.SetHit(false);
				MainRenderer->ChangeAnimation("Idle");
				StateValue = PlayerState::Idle;
				Ladder_Check = false;
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}
				
			
				//Capsule->SetWorldRotation({ 0.0f,Capsule->GetDir(),0.0f });
				if (GameEngineInput::IsPress('W', this) && GameEngineInput::IsPress(VK_SHIFT, this))
				{
					PlayerStates.ChangeState(PlayerState::Run);
					return;
				}

				

				

				if (GameEngineInput::IsPress('A', this))
				{
					MainRenderer->ChangeAnimation("Walk_Left");
					_Parent->ChangeState(PlayerState::Move);
					return;
				}


				if (GameEngineInput::IsPress('D', this))
				{
					MainRenderer->ChangeAnimation("Walk_Right");
					_Parent->ChangeState(PlayerState::Move);
					return;
				}

				if (GameEngineInput::IsPress('W', this))
				{
					//Player_Pos = Camera_Pos_X;
					MainRenderer->ChangeAnimation("Walk_Forward");
					_Parent->ChangeState(PlayerState::Move);
					return;
				}

				if (GameEngineInput::IsPress('S', this))
				{
					MainRenderer->ChangeAnimation("Walk_Behind");
					_Parent->ChangeState(PlayerState::Move);
					return;
				}
				if (GameEngineInput::IsPress(VK_SPACE, this))
				{
					_Parent->ChangeState(PlayerState::Back_Step);
					return;
				}

				if (GameEngineInput::IsPress('R', this) && Potion > 0)
				{
					MainRenderer->ChangeAnimation("Portion_Drink_01");
					_Parent->ChangeState(PlayerState::Portion_01);
					return;
				}

				if (GameEngineInput::IsPress(VK_LBUTTON, this))
				{
					Sword.ResetRecord();
					_Parent->ChangeState(PlayerState::Attack_01);
					return;
				}
				if (GameEngineInput::IsPress(VK_RBUTTON, this) && Stat.GetStamina() > 0)
				{
					_Parent->ChangeState(PlayerState::Shield_Idle);
					return;
				}
				if (GameEngineInput::IsPress(VK_CONTROL, this))
				{
					_Parent->ChangeState(PlayerState::Parrying);
					return;
				}

				
			};

		PlayerStates.CreateState(PlayerState::Idle, NewPara);
	}


	{
		CreateStateParameter NewPara;

		// 한번만 실행되는 겁니다.
		NewPara.Init = [=](class GameEngineState* _Parent)
			{
				// MainSpriteRenderer->ChangeAnimation("Idle");
			};


		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				StateValue = PlayerState::Move;
				Rotation_Check_X = false;
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Attack_Col->Off();
				Body_Col->On(); 
				Weapon_Actor->Getweapon()->On();
				//Player_Pos.X = Camera_Pos_X;
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				
				if (Rotation_Check_X == false)
				{
					if (Rock_On_Check != true)
					{
						if (Angle == 0)
						{
							Rotation_Check_X = true;
						}

						if (Angle > 0)
						{
							Rotation_Check_X_Plus = true;
						}
						else if (Angle < 0)
						{
							Rotation_Check_X_Mus = true;
						}

						if (Rotation_Check_X_Plus == true)
						{
							float4 add = float4::LerpClamp({ 0 ,0.0f }, { 500,0.0f }, _DeltaTime);

							Capsule->AddWorldRotation({ 0.0f,add.X });


							if (Angle <= 0)
							{

								Rotation_Check_X_Plus = false;
								Rotation_Check_X_Mus = false;
								Rotation_Check_X = true;
								Capsule->SetWorldRotation({ 0.0f,Cameracapsule->Capsule_02->GetDir() });
							}

						}
						else if (Rotation_Check_X_Mus == true)
						{
							float4 add = float4::LerpClamp({ 0 ,0.0f }, { 500,0.0f }, -_DeltaTime);
							Capsule->AddWorldRotation({ 0.0f,add.X });


							if (Angle >= 0)
							{
								Rotation_Check_X_Mus = false;
								Rotation_Check_X_Plus = false;
								Rotation_Check_X = true;
								Capsule->SetWorldRotation({ 0.0f,Cameracapsule->Capsule_02->GetDir() });
							}

						}
					}
				}

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetWorldRotation({ Rock_on_X,degree_X });
				}

				if (GameEngineInput::IsPress('W', this) && GameEngineInput::IsPress(VK_SPACE, this) && Rotation_Check_X ==true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Forward);
					return;
				}
				else if (GameEngineInput::IsPress('S', this) && GameEngineInput::IsPress(VK_SPACE, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Behind);
					return;
				}
				else if (GameEngineInput::IsPress('A', this) && GameEngineInput::IsPress(VK_SPACE, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Left);
					return;
				}

				else if (GameEngineInput::IsPress('D', this) && GameEngineInput::IsPress(VK_SPACE, this) && Rotation_Check_X == true  && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Right);
					return;
				}

				else if (GameEngineInput::IsPress('W', this) && GameEngineInput::IsPress(VK_SPACE, this) && Rock_On_Check == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Forward);
					return;
				}
				else if (GameEngineInput::IsPress('S', this) && GameEngineInput::IsPress(VK_SPACE, this)  && Rock_On_Check ==true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Behind);
					return;
				}
				else if (GameEngineInput::IsPress('A', this) && GameEngineInput::IsPress(VK_SPACE, this)  && Rock_On_Check == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Left);
					return;
				}
				else if (GameEngineInput::IsPress('D', this) && GameEngineInput::IsPress(VK_SPACE, this)  && Rock_On_Check == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Roll_Right);
					return;
				}


				else if (GameEngineInput::IsPress('W', this) && GameEngineInput::IsPress(VK_SHIFT, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					PlayerStates.ChangeState(PlayerState::Run);
					return; 
				}

			
				else if (GameEngineInput::IsPress(VK_RBUTTON, this) && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Shield_Idle);
					return;
				}
				else if (true == GameEngineInput::IsPress(VK_LBUTTON, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					Sword.ResetRecord();
					_Parent->ChangeState(PlayerState::Attack_01);
					return;
				}
				else if (true == GameEngineInput::IsPress(VK_LBUTTON, this) &&  Rock_On_Check == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					Sword.ResetRecord();
					_Parent->ChangeState(PlayerState::Attack_01);
					return;
				}

				else if (true == GameEngineInput::IsPress(VK_SPACE, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Back_Step);
					return;

				}

				else if (true == GameEngineInput::IsPress('W', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					MainRenderer->ChangeAnimation("Walk_Forward");
					//Capsule->MoveForce({ float4::FORWARD * Speed},Capsule->GetDir());
					//Capsule->SetWorldRotation({ 0.0f,abs(Angle),0.0f });
				}

				else if (true == GameEngineInput::IsPress('S', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					MainRenderer->ChangeAnimation("Walk_Behind");
					//Capsule->MoveForce({ float4::BACKWARD * Speed }, Capsule->GetDir());
					//Capsule->SetWorldRotation({ 0.0f,180.0f,0.0f });
				}
				else if (true == GameEngineInput::IsPress('A', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					MainRenderer->ChangeAnimation("Walk_Left");
					//Capsule->MoveForce({ float4::LEFT * Speed }, Capsule->GetDir());
					//Capsule->SetWorldRotation({ 0.0f,180.0f,0.0f });
				}
				else if (true == GameEngineInput::IsPress('D', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					MainRenderer->ChangeAnimation("Walk_Right");
					//Capsule->MoveForce({ float4::RIGHT * Speed }, Capsule->GetDir());
					//Capsule->SetWorldRotation({ 0.0f,180.0f,0.0f });
				}
			
				else if (true == GameEngineInput::IsPress('W', this)&& Rock_On_Check == true)
				{
					MainRenderer->ChangeAnimation("Walk_Forward");
					//Capsule->MoveForce({ float4::FORWARD * Speed } , degree_X);
				}

				else if (true == GameEngineInput::IsPress('S', this) && Rock_On_Check ==true)
				{				
					MainRenderer->ChangeAnimation("Walk_Behind");
					//Capsule->MoveForce({ float4::BACKWARD * Speed }, degree_X);
				}
				else if (true == GameEngineInput::IsPress('A', this) && Rock_On_Check == true)
				{
					MainRenderer->ChangeAnimation("Walk_Left");
					//Capsule->MoveForce({ float4::LEFT * Speed }, degree_X);
				}
				else if (true == GameEngineInput::IsPress('D', this) && Rock_On_Check == true)
				{
					MainRenderer->ChangeAnimation("Walk_Right");
					//Capsule->MoveForce({ float4::RIGHT * Speed }, degree_X);
				}
				

				else if (GameEngineInput::IsPress(VK_CONTROL, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Parrying);
					return;
				}

				else if (true == GameEngineInput::IsPress(VK_SPACE, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Back_Step);
					return;
				}



				if (true == GameEngineInput::IsPress('W', this) && true == GameEngineInput::IsPress('A', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					float4 Dir = { -1.0f, 0.0f,1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('W', this) && true == GameEngineInput::IsPress('D', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					float4 Dir = { 1.0f, 0.0f,1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('S', this) && true == GameEngineInput::IsPress('A', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					float4 Dir = { -1.0f, 0.0f,-1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('S', this) && true == GameEngineInput::IsPress('D', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					float4 Dir = { 1.0f, 0.0f,-1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('W', this) && true == GameEngineInput::IsPress('A', this) && Rock_On_Check == true)
				{
					float4 Dir = { -1.0f, 0.0f,1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('W', this) && true == GameEngineInput::IsPress('D', this) && Rock_On_Check == true)
				{
					float4 Dir = { 1.0f, 0.0f,1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('S', this) && true == GameEngineInput::IsPress('A', this)  && Rock_On_Check == true)
				{
					float4 Dir = { -1.0f, 0.0f,-1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}
				else if (true == GameEngineInput::IsPress('S', this) && true == GameEngineInput::IsPress('D', this) && Rock_On_Check == true)
				{
					float4 Dir = { 1.0f, 0.0f,-1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Cameracapsule->Capsule_02->GetDir());
				}

				else if (true == GameEngineInput::IsPress('W', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					
					Capsule->MoveForce({ float4::FORWARD * Speed }, Cameracapsule->Capsule_02->GetDir());
					
				}

				else if (true == GameEngineInput::IsPress('S', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					
					Capsule->MoveForce({ float4::BACKWARD * Speed }, Cameracapsule->Capsule_02->GetDir());
					
					}
				else if (true == GameEngineInput::IsPress('A', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
				
					Capsule->MoveForce({ float4::LEFT * Speed }, Cameracapsule->Capsule_02->GetDir());
				
					}
				else if (true == GameEngineInput::IsPress('D', this) && Rotation_Check_X == true && Rock_On_Check == false)
				{
					
					Capsule->MoveForce({ float4::RIGHT * Speed }, Cameracapsule->Capsule_02->GetDir());
				
				}

				else if (true == GameEngineInput::IsPress('W', this) && Rock_On_Check == true)
				{
					Capsule->MoveForce({ float4::FORWARD * Speed });
				}

				else if (true == GameEngineInput::IsPress('S', this) && Rock_On_Check == true)
				{		
					Capsule->MoveForce({ float4::BACKWARD * Speed });
				}	
				else if (true == GameEngineInput::IsPress('A', this) && Rock_On_Check == true)
				{		
					Capsule->MoveForce({ float4::LEFT * Speed });
				}
				else if (true == GameEngineInput::IsPress('D', this) && Rock_On_Check == true)
				{	
					Capsule->MoveForce({ float4::RIGHT * Speed });
				}

				else if (GameEngineInput::IsUp('A', this) )
				{
					Rotation_Check_X = false;
					MainRenderer->ChangeAnimation("Left_Stop");
					PlayerStates.ChangeState(PlayerState::Move_Stop);
					return;
				}
				else if (GameEngineInput::IsUp('D', this) )
				{
					Rotation_Check_X = false;
					MainRenderer->ChangeAnimation("Right_Stop");
					PlayerStates.ChangeState(PlayerState::Move_Stop);
					return;
				}
				else if (GameEngineInput::IsUp('W', this) )
				{
					//Camera_Pos_X = Player_Pos.X;
					Rotation_Check_X = false;
					MainRenderer->ChangeAnimation("Forward_Stop");
					PlayerStates.ChangeState(PlayerState::Move_Stop);
					return;
				}
				else if (GameEngineInput::IsUp('S', this))
				{
					Rotation_Check_X = false;
					MainRenderer->ChangeAnimation("Behind_Stop");
					PlayerStates.ChangeState(PlayerState::Move_Stop);
					return;
				}
				
				if (GameEngineInput::IsPress('R', this) && Potion > 0 && Rock_On_Check == true)
				{
					Rotation_Check_X = false;
					MainRenderer->ChangeAnimation("Portion_Drink_01");
					_Parent->ChangeState(PlayerState::Portion_01);
					return;
				}

				else if (GameEngineInput::IsPress('R', this) && Potion > 0 && Rotation_Check_X == true)
				{
					Rotation_Check_X = false;
					MainRenderer->ChangeAnimation("Portion_Drink_01");
					_Parent->ChangeState(PlayerState::Portion_01);
					return;
				}

				if (GameEngineInput::IsPress(VK_CONTROL, this) && Rock_On_Check == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Parrying);
					return;
				}

				else if (GameEngineInput::IsPress(VK_CONTROL, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Parrying);
					return;
				}


				if (GameEngineInput::IsPress(VK_RBUTTON, this) && Rock_On_Check == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Shield_Idle);
					return;
				}

				else if (GameEngineInput::IsPress(VK_RBUTTON, this) && Rotation_Check_X == true && Stat.GetStamina() > 0)
				{
					Rotation_Check_X = false;
					_Parent->ChangeState(PlayerState::Shield_Idle);
					return;
				}
				
				


				

			};

		PlayerStates.CreateState(PlayerState::Move, NewPara);
	}


	{
		CreateStateParameter NewPara;

		// 한번만 실행되는 겁니다.
		NewPara.Init = [=](class GameEngineState* _Parent)
			{
				// MainSpriteRenderer->ChangeAnimation("Idle");
			};


		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				//Capsule->ResetMove(Enum_Axies::All);
				
				StateValue = PlayerState::Move_Stop;
				
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
			
				if (Rock_On_Check == true)
				{


					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetWorldRotation({ Rock_on_X,degree_X });
				}

				if (GameEngineInput::IsPress(VK_CONTROL, this) && Stat.GetStamina() > 0)
				{
					_Parent->ChangeState(PlayerState::Parrying);
					return;
				}

				if (GameEngineInput::IsPress(VK_RBUTTON, this) && Stat.GetStamina() > 0)
				{
					_Parent->ChangeState(PlayerState::Shield_Idle);
					return;
				}

				if (true == GameEngineInput::IsPress(VK_LBUTTON, this) && Stat.GetStamina() > 0)
				{
					Sword.ResetRecord();
					_Parent->ChangeState(PlayerState::Attack_01);
					return;
				}
			
				if (MainRenderer->GetCurAnimationFrame() >= 35)
				{
					
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				/*if (true == GameEngineInput::IsPress('W', this) && true == GameEngineInput::IsPress('A', this))
				{
					float4 Dir = { -1.0f, 0.0f,1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Capsule->GetDir());

					PlayerStates.ChangeState(PlayerState::Move);

					return;
				}
				else if (true == GameEngineInput::IsPress('W', this) && true == GameEngineInput::IsPress('D', this))
				{
					float4 Dir = { 1.0f, 0.0f,1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Capsule->GetDir());

					PlayerStates.ChangeState(PlayerState::Move);

					return;
				}
				else if (true == GameEngineInput::IsPress('S', this) && true == GameEngineInput::IsPress('A', this))
				{
					float4 Dir = { -1.0f, 0.0f,-1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Capsule->GetDir());

					PlayerStates.ChangeState(PlayerState::Move);

					return;
				}
				else if (true == GameEngineInput::IsPress('S', this) && true == GameEngineInput::IsPress('D', this))
				{
					float4 Dir = { 1.0f, 0.0f,-1.0f };

					Dir.Normalize();

					Capsule->MoveForce({ Dir * Speed }, Capsule->GetDir());
					PlayerStates.ChangeState(PlayerState::Move);

					return; 
				}*/



				if (GameEngineInput::IsPress('A', this))
				{
					
					MainRenderer->ChangeAnimation("Walk_Left");
					PlayerStates.ChangeState(PlayerState::Move);
					return;
				}
				if (GameEngineInput::IsPress('D', this))
				{
				
					MainRenderer->ChangeAnimation("Walk_Right");
					PlayerStates.ChangeState(PlayerState::Move);
					return;
				}
				if (GameEngineInput::IsPress('W', this))
				{
				
					MainRenderer->ChangeAnimation("Walk_Forward");
					PlayerStates.ChangeState(PlayerState::Move);
					return;
				}
				if (GameEngineInput::IsPress('S', this))
				{
				
					MainRenderer->ChangeAnimation("Walk_Behind");
					PlayerStates.ChangeState(PlayerState::Move);
					return;
				}



				
			};

		PlayerStates.CreateState(PlayerState::Move_Stop, NewPara);
	}


	{
		CreateStateParameter NewPara;

		// 한번만 실행되는 겁니다.
		NewPara.Init = [=](class GameEngineState* _Parent)
			{

			};


		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				StateValue = PlayerState::Slow_Walk;
				MainRenderer->ChangeAnimation("Slow_Walk_Forward");
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				

			};

		PlayerStates.CreateState(PlayerState::Slow_Walk, NewPara);
	}

/*{
	CreateStateParameter NewPara;





	NewPara.Start = [=](class GameEngineState* _Parent)
	{
			MainRenderer->ChangeAnimation("Run");
	};


	NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
	{



		if (GameEngineInput::IsPress('W', this) && GameEngineInput::IsPress(VK_SHIFT, this))
		{
			GetFBXRenderer()->Transform.AddLocalPosition({ 0.0f,0.0f,0.0f });
		}

		if (GameEngineInput::IsUp('W', this) )
		{
			MainRenderer->ChangeAnimation("Forward_Stop");
			PlayerStates.ChangeState(PlayerState::Move_Stop);
			return;
		}

		if (GameEngineInput::IsUp(VK_SHIFT, this))
		{
			MainRenderer->ChangeAnimation("Forward_Stop");
			PlayerStates.ChangeState(PlayerState::Move);
			return;
		}

	};

	PlayerStates.CreateState(PlayerState::Run, NewPara);
}*/

{
	CreateStateParameter NewPara;

	NewPara.Start = [=](class GameEngineState* _Parent)
		{
			Rotation_Check_X = true;
			MainRenderer->ChangeAnimation("Run");
			StateValue = PlayerState::Run;
		};


	NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
		{
			// 임시 지정 값 
			 float Stamina = _DeltaTime * 10.0f;

			 Stat.AddStamina(-Stamina);


			 if (Stat.GetStamina() <= 0)
			 {
				 PlayerStates.ChangeState(PlayerState::Idle);
				 return; 
			 }


			if (GameEngineInput::IsPress('W', this) && GameEngineInput::IsPress(VK_SHIFT, this))
			{
				Capsule->MoveForce({ float4::FORWARD * Speed * 1.5 }, Capsule->GetDir());
			}

			if (GameEngineInput::IsPress('R', this) && Potion > 0 )
			{
				MainRenderer->ChangeAnimation("Portion_Drink_01");
				_Parent->ChangeState(PlayerState::Portion_01);
				return;
			}

			if (GameEngineInput::IsUp('W', this))
			{
				MainRenderer->ChangeAnimation("Forward_Stop");
				PlayerStates.ChangeState(PlayerState::Move_Stop);
				return;
			}

			if (GameEngineInput::IsUp(VK_SHIFT, this))
			{
				MainRenderer->ChangeAnimation("Forward_Stop");
				PlayerStates.ChangeState(PlayerState::Move);
				return;
			}

		};

	PlayerStates.CreateState(PlayerState::Run, NewPara);
	}

{
	CreateStateParameter NewPara;

	NewPara.Start = [=](class GameEngineState* _Parent)
		{
			Sword.ResetRecord();
			MainRenderer->ChangeAnimation("Attack_01");
			StateValue = PlayerState::StaminaCheck;
			Capsule->SetWorldRotation({ 0.0f,Cameracapsule->Capsule_02->GetDir()});
			Stat.AddStamina(-20);		
			Attack_Check = false;
		};


	NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
		{
			Hit.SetHit(false);

			if (MainRenderer->GetCurAnimationFrame() >= 13)
			{
				if (MainRenderer->GetCurAnimationFrame() <= 16)
				{
					Attack_Col->On();
				}
			}
			if  (MainRenderer->GetCurAnimationFrame() > 16)
			{
				Attack_Col->Off();
			}



				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}
				

				if (MainRenderer->GetCurAnimationFrame() < 30)
				{
					if (GameEngineInput::IsDown(VK_LBUTTON, this))
					{
						Attack_Check = true; 
					}
				}	

				if (MainRenderer->IsCurAnimationEnd())
				{
					Attack_Col->Off();
					Attack_Check = false;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (Attack_Check == true && MainRenderer->GetCurAnimationFrame() > 20 && Stat.GetStamina() > 0)
				{
					Sword.ResetRecord();
					Attack_Check = false;
					PlayerStates.ChangeState(PlayerState::Attack_02);
					return;
				}

				if (MainRenderer->GetCurAnimationFrame() > 15 && Attack_Check==false)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();
		
						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();
				
						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();
						
						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
				
		};

		PlayerStates.CreateState(PlayerState::Attack_01, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Sword.ResetRecord();
				Attack_Check = false;
				MainRenderer->ChangeAnimation("Attack_02");
				StateValue = PlayerState::StaminaCheck;
				Capsule->SetWorldRotation({ 0.0f,Cameracapsule->Capsule_02->GetDir() });
				Stat.AddStamina(-20);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				if (MainRenderer->GetCurAnimationFrame() >= 14)
				{
					if (MainRenderer->GetCurAnimationFrame() <= 18)
					{
						Attack_Col->On();
					}

				}
				if (MainRenderer->GetCurAnimationFrame() > 18)
				{
					Attack_Col->Off();
				}


				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}


				if (MainRenderer->GetCurAnimationFrame() < 30)
				{
					if (GameEngineInput::IsDown(VK_LBUTTON, this))
					{
						Attack_Check = true;
					}
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					Attack_Col->Off();
					Attack_Check = false;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (Attack_Check == true && MainRenderer->GetCurAnimationFrame() > 20 && Stat.GetStamina() > 0)
				{

					Attack_Check = false;
					Sword.ResetRecord();
					PlayerStates.ChangeState(PlayerState::Attack_03);
					return;
				}

				if (MainRenderer->GetCurAnimationFrame() > 15 && Attack_Check ==false)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();
						Rotation_Check_X = true;
						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
				
			};

		PlayerStates.CreateState(PlayerState::Attack_02, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Sword.ResetRecord();
				Attack_Check = false;
				MainRenderer->ChangeAnimation("Attack_03");
				StateValue = PlayerState::StaminaCheck;
				Capsule->SetWorldRotation({ 0.0f,Cameracapsule->Capsule_02->GetDir() });
				Stat.AddStamina(-20);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				if (MainRenderer->GetCurAnimationFrame() >= 13)
				{
					if (MainRenderer->GetCurAnimationFrame() <= 15)
					{
						Attack_Col->On();
					}

				}
				if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					Attack_Col->Off();
				}

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}


				if (MainRenderer->GetCurAnimationFrame() < 30)
				{
					if (GameEngineInput::IsDown(VK_LBUTTON, this))
					{
						Attack_Check = true;
					}
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					Attack_Col->Off();
					Attack_Check = false;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}


				else if (Attack_Check == true && MainRenderer->GetCurAnimationFrame() > 20 && Stat.GetStamina() > 0)
				{
					Attack_Check = false;
					Sword.ResetRecord();
					PlayerStates.ChangeState(PlayerState::Attack_04);
					return;
				}

				if (MainRenderer->GetCurAnimationFrame() > 15 && Attack_Check==false)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();
						
						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();
						
						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
				
			};

		PlayerStates.CreateState(PlayerState::Attack_03, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Sword.ResetRecord();
				Attack_Check = false;
				MainRenderer->ChangeAnimation("Attack_04");
				StateValue = PlayerState::StaminaCheck;
				Capsule->SetWorldRotation({ 0.0f,Cameracapsule->Capsule_02->GetDir() });
				Stat.AddStamina(-20);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				if (MainRenderer->GetCurAnimationFrame() >= 16)
				{
					if (MainRenderer->GetCurAnimationFrame() <= 19)
					{
						Attack_Col->On();
					}

				}

				if (MainRenderer->GetCurAnimationFrame() > 19)
				{ 
					Attack_Col->Off();
				}
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}


				if (MainRenderer->GetCurAnimationFrame() < 30)
				{
					if (GameEngineInput::IsDown(VK_LBUTTON, this))
					{
						Attack_Check = true;
					}
				}

				if (Attack_Check == false && MainRenderer->IsCurAnimationEnd())
				{
					Attack_Col->Off();
					Attack_Check = false;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (Attack_Check == true && MainRenderer->GetCurAnimationFrame() > 25 && Stat.GetStamina() > 0)
				{
					Attack_Check = false;
					Sword.ResetRecord();
					PlayerStates.ChangeState(PlayerState::Attack_01);
					return;
				}

				if (MainRenderer->GetCurAnimationFrame() > 20 && Attack_Check ==false)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();
					
						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();
				
						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}

				
			};

		PlayerStates.CreateState(PlayerState::Attack_04, NewPara);
	}



	

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Portion_Drink_01");
				Potion -=1; 
				Stat.AddHp(400);
				Weapon_Actor->Getweapon()->Off(); 
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Portion_02);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Portion_01, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Portion_Drink_02");
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (MainRenderer->GetCurAnimationFrame() > 10)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{


						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{


						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{


						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{


						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}
				}




				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Portion_03);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Portion_02, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Portion_Drink_03");
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Weapon_Actor->Getweapon()->On();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

			}; 

		PlayerStates.CreateState(PlayerState::Portion_03, NewPara);
	}
	
	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Roll_Behind");
				StateValue = PlayerState::StaminaCheck;
				Stat.AddStamina(-60.0f);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });

					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::BACKWARD * Speed }, degree_X);
					}
				}

				else 
				{
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::BACKWARD * Speed });
					}
				}

				if (MainRenderer->GetCurAnimationFrame() < 15)
				{
					Body_Col->Off(); 
				}
				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					Body_Col->On();
				}







				if (MainRenderer->GetCurAnimationFrame() > 20)
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Roll_Behind, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Roll_Forward");
				StateValue = PlayerState::StaminaCheck;
				Stat.AddStamina(-60.0f);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				

			    if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::FORWARD * Speed }, degree_X);
					}
				}

				else
				{
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::FORWARD * Speed });
					}
				}


				if (MainRenderer->GetCurAnimationFrame() < 15)
				{
					Body_Col->Off();
				}
				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					Body_Col->On();
				}


				if (MainRenderer->GetCurAnimationFrame() > 20)
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}


			};






		PlayerStates.CreateState(PlayerState::Roll_Forward, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Roll_Right");
				StateValue = PlayerState::StaminaCheck;
				Stat.AddStamina(-60.0f);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
			
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::LEFT * Speed  });
					}
				}

				else
				{
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::LEFT * Speed });
					}

					
				}

				if (MainRenderer->GetCurAnimationFrame() < 23)
				{
					Body_Col->Off();
				}
				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					Body_Col->On();
				}


				if (MainRenderer->GetCurAnimationFrame() > 23)
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Roll_Left, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Roll_Left");
				StateValue = PlayerState::StaminaCheck;
				Stat.AddStamina(-60.0f);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::RIGHT * Speed  });
					}
				}

				else
				{
					if (MainRenderer->GetCurAnimationFrame() < 20)
					{
						Capsule->MoveForce({ float4::RIGHT * Speed  });
					}
				}


				if (MainRenderer->GetCurAnimationFrame() < 23)
				{
					Body_Col->Off();
				}
				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					Body_Col->On();
				}



				if (MainRenderer->GetCurAnimationFrame() > 23)
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Roll_Right, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Back_Step");
				StateValue = PlayerState::StaminaCheck;
				Stat.AddStamina(-60.0f);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });

					if (MainRenderer->GetCurAnimationFrame() < 10)
					{
						Capsule->MoveForce({ float4::BACKWARD * Speed * float(1.2) });
					}
				}
				else
				{
					if (MainRenderer->GetCurAnimationFrame() < 10)
					{
						Capsule->MoveForce({ float4::BACKWARD * Speed * float(1.2) });
					}
				}
				if (MainRenderer->GetCurAnimationFrame() > 20)
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Back_Step, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Top_Shield_Col->On();
				Shield_Col->On();
				Hit.SetHit(false);
				MainRenderer->ChangeAnimation("Shield_Idle");
				StateValue = PlayerState::Shield_Idle;
				
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Stat.GetStamina() <= 0)
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return; 
				}
				
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				

				if (true == GameEngineInput::IsUp(VK_RBUTTON, this))
				{
					Top_Shield_Col->Off();
					Shield_Col->Off(); 
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

			};

		PlayerStates.CreateState(PlayerState::Shield_Idle, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Shield_Move");
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (true == GameEngineInput::IsPress(VK_RBUTTON, this) && true == GameEngineInput::IsPress('W', this))
				{
					PlayerStates.ChangeState(PlayerState::Shield_Move);
					return;
				}

				 if (true == GameEngineInput::IsUp(VK_RBUTTON, this))
				{
					 MainRenderer->ChangeAnimation("Walk_Forward");
				    PlayerStates.ChangeState(PlayerState::Move);
					return;
				}
				if (true == GameEngineInput::IsUp('W', this))
				{
					PlayerStates.ChangeState(PlayerState::Shield_Idle);
					return;
				}

			};

		PlayerStates.CreateState(PlayerState::Shield_Move, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Parrying");
				Hit.SetHit(false);
				StateValue = PlayerState::Parrying;
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->GetCurAnimationFrame() < 14)
				{
					Top_Shield_Col->On();
					Shield_Col->On();
				}
				if (MainRenderer->GetCurAnimationFrame() > 14)
				{
					Top_Shield_Col->Off();
					Shield_Col->Off();
				}

				

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return; 
				}



			};

		PlayerStates.CreateState(PlayerState::Parrying, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Attack_Col->Off();

				StateValue = PlayerState::RockOn;

				//MainRenderer->ChangeAnimation("Idle");

				Rock_On_Check_X = false;
				Rotation_Check_X = false;
				Rotation_Check_X_Plus = false;
				Rotation_Check_X_Mus = false;


				Rock_On_Check_Y = false;
				Rotation_Check_Y = false;
				Rotation_Check_Y_Plus = false;
				Rotation_Check_Y_Mus = false;

				

				Rock_On_Player = false;
				Rotation_Player_Check = false;
				Rotation_Player_Plus = false;
				Rotation_Player_Mus = false;

				Rock_On_Check = true;

				error_Number_X = 0;
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
		{


			

				if (GetTargetPointer() != nullptr)
				{
					


					if (Rock_On_Player == false)
					{
						Actor_test->Transform.SetWorldRotation({ 20.0f,Capsule->GetDir() });

						float MonsterAngle;
						float4 TargetPos = GetTargetPointer()->Transform.GetWorldPosition();
						float4 MyPos = GetLevel()->GetMainCamera()->Transform.GetWorldPosition();

						// Y축 고려 X
						TargetPos.Y = MyPos.Y = 0.f;

						float4 FrontVector = float4(0.f, 0.f, -1.f, 0.f);
						FrontVector.VectorRotationToDegY(Cameracapsule->Capsule_02->GetDir());

						float4 LocationVector = (TargetPos - MyPos).NormalizeReturn();

						float4 Angle_ = DirectX::XMVector3AngleBetweenNormals(FrontVector.DirectXVector, LocationVector.DirectXVector);

						float4 RotationDir = DirectX::XMVector3Cross(FrontVector.DirectXVector, LocationVector.DirectXVector);

						MonsterAngle = Angle_.X * GameEngineMath::R2D;

						if (0.0f <= RotationDir.Y)
						{
							adad = true;
						}
						else
						{
							adad = false;
						}



						if (GetTargetAngle() == 0)
						{
							Rotation_Player_Check = true;
						}

						else if (GetTargetAngle() > 0)
						{
							Rotation_Player_Plus = true;
						}

						else if (GetTargetAngle() < 0)
						{
							Rotation_Player_Mus = true;
						}

						Rock_On_Player = true;

					}

					if (Rotation_Player_Plus == true)
					{
						float4 Lerp = float4::LerpClamp({ 0.0f,0.0f }, { 200.0f,0.0f },_DeltaTime);


						Capsule->AddWorldRotation({ 0.0f,Lerp.X });
						Actor_test->Transform.AddLocalRotation({ 0.0f,Lerp.X });
					



						if (GetTargetAngle() <= 0)
						{
							Rotation_Player_Check = true;
							Rotation_Player_Mus = false;
							Rotation_Player_Plus = false;
						}


					}
					else if (Rotation_Player_Mus == true)
					{

						float4 Lerp = float4::LerpClamp({ 0.0f,0.0f }, { 200.0f,0.0f },-_DeltaTime);


						Capsule->AddWorldRotation({ 0.0f,Lerp.X });
						Actor_test->Transform.AddLocalRotation({ 0.0f,Lerp.X });

						if (GetTargetAngle() >= 0)
						{
							Rotation_Player_Check = true;
							Rotation_Player_Mus = false;
							Rotation_Player_Plus = false;
						}
					}





					if (Rock_On_Check_X == false)
					{
						/*if (degree_X == Actor_test->Transform.GetWorldRotationEuler().Y)
						{
							Rotation_Check_X = true;
						}

						if (degree_X > Actor_test->Transform.GetWorldRotationEuler().Y)
						{
							Rotation_Check_X_Plus = true;
						}
						else if (degree_X < Actor_test->Transform.GetWorldRotationEuler().Y)
						{
							Rotation_Check_X_Mus = true;
						}*/




						if (adad == true)
						{
							Rotation_Check_X_Plus = true;
						}
						else if (adad == false)
						{
							Rotation_Check_X_Mus = true;
						}

						Rock_On_Check_X = true;
					}

					if (Rotation_Check_X_Plus == true)
					{

						float4 Lerp = float4::LerpClamp({ 0.0f,0.0f }, { 200.0f,0.0f }, _DeltaTime);


						/*if (degree_X > Actor_test->Transform.GetWorldRotationEuler().Y)
						{
							Actor_test->Transform.AddWorldRotation({ 0.0f,Lerp.X });
						}
					
						else if (degree_X < Actor_test->Transform.GetWorldRotationEuler().Y)
						{
							Rotation_Check_X_Plus = false;
							Rotation_Check_X_Mus = false;
							Rotation_Check_X = true;
						}*/

					}
					else if (Rotation_Check_X_Mus == true)
					{

						float4 Lerp = float4::LerpClamp({ 0.0f,0.0f }, { 200.0f,0.0f }, -_DeltaTime);


						/*Actor_test->Transform.AddWorldRotation({ 0.0f,Lerp.X });
						
					
						if (degree_X < Actor_test->Transform.GetWorldRotationEuler().Y)
						{
							Actor_test->Transform.AddWorldRotation({ 0.0f,Lerp.X });
						}*/



						/*else if (degree_X > Actor_test->Transform.GetWorldRotationEuler().Y)
						{

							Rotation_Check_X_Mus = false;
							Rotation_Check_X_Plus = false;
							Rotation_Check_X = true;

						}*/

					}








					if (Rock_On_Check_Y == false)
					{
						if (Actor_test->Transform.GetWorldRotationEuler().X == 20)
						{
							Rotation_Check_Y = true;
						}


						if (Actor_test->Transform.GetWorldRotationEuler().X <= 20)
						{
							Rotation_Check_Y_Plus = true;
						}
						else if (Actor_test->Transform.GetWorldRotationEuler().X >= 20)
						{
							Rotation_Check_Y_Mus = true;
						}

						Rock_On_Check_Y = true;
					}


					/*if (Rotation_Check_Y_Plus == true)
					{
						float4 Lerp = float4::LerpClamp({ 0.0f,0.0f }, { 200.0f,0.0f }, _DeltaTime);


						Actor_test->Transform.AddWorldRotation({ Lerp.X,0.0f });



						if (Actor_test->Transform.GetWorldRotationEuler().X >= 20)
						{

							Rotation_Check_Y_Plus = false;
							Rotation_Check_Y_Mus = false;
							Rotation_Check_Y = true;
						}

					}

					else if (Rotation_Check_Y_Mus == true)
					{
						float4 Lerp = float4::LerpClamp({ 0.0f,0.0f }, { 200.0f,0.0f }, -_DeltaTime);


						Actor_test->Transform.AddWorldRotation({ Lerp.X,0.0f });


			
						if (Actor_test->Transform.GetWorldRotationEuler().X <= 20)
						{
							Rotation_Check_Y_Mus = false;
							Rotation_Check_Y_Plus = false;
							Rotation_Check_Y = true;


						}

					}*/

				}
				/*else
				{
					Rock_On_Check = false;
					return;
				}*/
				
			


				if (Rotation_Player_Check == true)
				{
					/*if (degree_X < 0 && Actor_test->Transform.GetWorldRotationEuler().Y < 0)
					{
						if (Actor_test->Transform.GetWorldRotationEuler().Y > degree_X)
						{
							error_Number_X = abs(Actor_test->Transform.GetWorldRotationEuler().Y) - abs(degree_X);
						}

						else if (Actor_test->Transform.GetWorldRotationEuler().Y < degree_X)
						{
							error_Number_X = abs(degree_X) - abs(Actor_test->Transform.GetWorldRotationEuler().Y);
						}
					}

					else if(degree_X > 0 && Actor_test->Transform.GetWorldRotationEuler().Y > 0)
					{

						if (Actor_test->Transform.GetWorldRotationEuler().Y > degree_X)
						{
							error_Number_X = Actor_test->Transform.GetWorldRotationEuler().Y - degree_X;
						}

						else if (Actor_test->Transform.GetWorldRotationEuler().Y < degree_X)
						{
							error_Number_X = degree_X - Actor_test->Transform.GetWorldRotationEuler().Y;
						}
					}

					else if (degree_X > 0 && Actor_test->Transform.GetWorldRotationEuler().Y < 0)
					{
						error_Number_X = degree_X + Actor_test->Transform.GetWorldRotationEuler().Y;
					}
					else if (degree_X < 0 && Actor_test->Transform.GetWorldRotationEuler().Y > 0)
					{
						error_Number_X = Actor_test->Transform.GetWorldRotationEuler().Y + degree_X;
					}*/


				
					
					Rock_on_X = Actor_test->Transform.GetWorldRotationEuler().X;



					Actor_test->Transform.GetWorldRotationEuler(); 


				
					//error_Number_X = Actor_test->Transform.GetWorldRotationEuler()

						PlayerStates.ChangeState(PlayerState::Move);
						return;
				}
			};

		PlayerStates.CreateState(PlayerState::RockOn, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Hit_Forward",true);
				Top_Shield_Col->Off();
				Shield_Col->Off(); 
				Hit.SetHit(false);
				StateValue = PlayerState::Forward_Hit;
				Weapon_Actor->Getweapon()->On();
				Sword.ResetRecord();

			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}		

				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}

			};

		PlayerStates.CreateState(PlayerState::Forward_Hit, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Hit_Behind", true);
				Hit.SetHit(false);
				StateValue = PlayerState::Backward_Hit;
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
				//Capsule->SetWorldRotation({ 0.0f,degree_X });
			};

		PlayerStates.CreateState(PlayerState::Backward_Hit, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Hit_right", true);
				Hit.SetHit(false);
				StateValue = PlayerState::Right_Hit;
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}

				//Capsule->SetWorldRotation({ 0.0f,degree_X });
			};

		PlayerStates.CreateState(PlayerState::Right_Hit, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Hit_Left", true);
				StateValue = PlayerState::Left_Hit;
				Hit.SetHit(false);
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 15)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}


				//Capsule->SetWorldRotation({ 0.0f,degree_X });
			};

		PlayerStates.CreateState(PlayerState::Left_Hit, NewPara);
	}






	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Middle_Hit_Forward", true);
				Hit.SetHit(false);
				StateValue = PlayerState::Forward_Middle_Hit;
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 30)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						Attack_Col->Off();

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}

			};

		PlayerStates.CreateState(PlayerState::Forward_Middle_Hit, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Middle_Hit_Behind", true);
				Hit.SetHit(false);
				StateValue = PlayerState::Backward_Middle_Hit;
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 30)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
		
			};

		PlayerStates.CreateState(PlayerState::Backward_Middle_Hit, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Middle_Hit_Right", true);
				Hit.SetHit(false);
				StateValue = PlayerState::Right_Middle_Hit;
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
				else if (MainRenderer->GetCurAnimationFrame() > 30)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
			
			};

		PlayerStates.CreateState(PlayerState::Right_Middle_Hit, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Middle_Hit_Left", true);
				StateValue = PlayerState::Left_Middle_Hit;
				Hit.SetHit(false);
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 30)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
					

						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
					

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
			};

		PlayerStates.CreateState(PlayerState::Left_Middle_Hit, NewPara);
	}

	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("Hit_Down");
				StateValue = PlayerState::HitDown;
				Weapon_Actor->Getweapon()->On();
				Hit.SetHit(false);
				Body_Col->Off();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Sword.ResetRecord();
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				

				if (MainRenderer->GetCurAnimationFrame() > 40)
				{
					Body_Col->On();
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					Body_Col->On();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				else if (MainRenderer->GetCurAnimationFrame() > 50)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{
						
						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{
						

						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
			};

		PlayerStates.CreateState(PlayerState::HitDown, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("String_Hit_Forward");
				Hit.SetHit(false);
				StateValue = PlayerState::Forward_Big_Hit;
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Body_Col->Off(); 
				Sword.ResetRecord();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				
				
				Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });

					if (MainRenderer->GetCurAnimationFrame() < 15)
					{
						Capsule->MoveForce({ 0.0f,0.0f,-600.0f }, degree_X);

					}
				}
				else
				{
					if (MainRenderer->GetCurAnimationFrame() < 15)
					{
						Capsule->MoveForce({ 0.0f,0.0f,-600.0f }, Capsule->GetDir());
					}
				}

				if (MainRenderer->GetCurAnimationFrame() > 70)
				{
					Body_Col->On();
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					Body_Col->On();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}


				else if (MainRenderer->GetCurAnimationFrame() > 80)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{


						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{


						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{


						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{


						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}

			};

		PlayerStates.CreateState(PlayerState::Forward_Big_Hit, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				// 애니메이션 터짐
				Attack_Col->Off();
				MainRenderer->ChangeAnimation("String_Hit_Behind");
				StateValue = PlayerState::Backward_Big_Hit;
				Hit.SetHit(false);
				Weapon_Actor->Getweapon()->On();
				Top_Shield_Col->Off();
				Shield_Col->Off();
				Body_Col->Off();
				Sword.ResetRecord(); 
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

				

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });

					if (MainRenderer->GetCurAnimationFrame() < 15)
					{
						Capsule->MoveForce({ 0.0f,0.0f,600.0f }, degree_X);

					}
				}
				else
				{
					if (MainRenderer->GetCurAnimationFrame() < 15)
					{
						Capsule->MoveForce({ 0.0f,0.0f,600.0f }, Capsule->GetDir());
					}
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					Body_Col->On();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				if (MainRenderer->GetCurAnimationFrame() > 70)
				{
					Body_Col->On();
				}

				else if (MainRenderer->GetCurAnimationFrame() > 80)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{


						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{


						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{


						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{


						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

				}
				
			};

		PlayerStates.CreateState(PlayerState::Backward_Big_Hit, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Up_Start");
				Capsule->GravityOff();
				Rock_On_Check = true;
				//Body_Col->Off();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Rock_On_Check = false;
					Camera_Pos_Y = Actor_test->Transform.GetWorldRotationEuler().X;
					Player_Pos.X = Actor_test->Transform.GetWorldRotationEuler().Y;
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					if (GameEngineInput::IsPress('W', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Up_Left);
						return;
					}

					
				}
			};

		PlayerStates.CreateState(PlayerState::ladder_Up_Start, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				StateValue = PlayerState::ladder_Up_Left;
				MainRenderer->ChangeAnimation("ladder_Up_Left");
				Capsule->GravityOff();
				//Body_Col->Off();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
			
				Body_Col->CollisionEvent(Enum_CollisionOrder::LabberMiddle, Labber_Middle_Event);

				

				if (MainRenderer->IsCurAnimationEnd())
				{
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					if (GameEngineInput::IsPress('W', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Up_Right);
						return;
					}

					if (GameEngineInput::IsPress('S', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Down_Right);
						return;
					}
					
				}
				


			};

		PlayerStates.CreateState(PlayerState::ladder_Up_Left, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				StateValue = PlayerState::ladder_Up_Right;
				MainRenderer->ChangeAnimation("ladder_Up_Right");
				Capsule->GravityOff();
				//Body_Col->Off();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Body_Col->CollisionEvent(Enum_CollisionOrder::LabberMiddle, Labber_Middle_Event);

				if (MainRenderer->IsCurAnimationEnd())
				{
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					if (GameEngineInput::IsPress('W', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Up_Left);
						return;
					}	

					if (GameEngineInput::IsPress('S', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Down_Left);
						return;
					}


				}
				


			};

		PlayerStates.CreateState(PlayerState::ladder_Up_Right, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Up_Stop_Left");
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{		
					Ladder_Check = false;
					Body_Col->On();
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					Capsule->GravityOn();
					Rabber_Collision_Check = false;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				

			};

		PlayerStates.CreateState(PlayerState::ladder_Up_Stop_Left, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Up_Stop_Right");
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Ladder_Check = false;
					Body_Col->On();
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					Capsule->GravityOn();
					Rabber_Collision_Check = false;;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::ladder_Up_Stop_Right, NewPara);
	}



	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Down_Start");
				Capsule->GravityOff();
				//Body_Col->Off();
				Rock_On_Check = true;
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Camera_Pos_Y = Actor_test->Transform.GetWorldRotationEuler().X;
					Player_Pos.X = Actor_test->Transform.GetWorldRotationEuler().Y;
					Rock_On_Check = false;

					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					PlayerStates.ChangeState(PlayerState::ladder_Down_Left);
					return;
					
				}
			};

		PlayerStates.CreateState(PlayerState::ladder_Down_Start, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Down_Left");
				//Body_Col->Off();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (Body_Col->Collision(Enum_CollisionOrder::LadderBot))
				{
					PlayerStates.ChangeState(PlayerState::ladder_Down_Stop_Left);
					return;
				}
				if (MainRenderer->IsCurAnimationEnd())
				{
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);



					if (GameEngineInput::IsPress('W', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Up_Right);
						return;
					}
					if (GameEngineInput::IsPress('S', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Down_Right);
						return;
					}
				}



			};

		PlayerStates.CreateState(PlayerState::ladder_Down_Left, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Down_Right");
				//Body_Col->Off();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (Body_Col->Collision(Enum_CollisionOrder::LadderBot))
				{
					PlayerStates.ChangeState(PlayerState::ladder_Down_Stop_Right);
					return;
				}

				


				if (MainRenderer->IsCurAnimationEnd())
				{

					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					if (GameEngineInput::IsPress('W', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Up_Left);
						return;
					}
					if (GameEngineInput::IsPress('S', this))
					{
						PlayerStates.ChangeState(PlayerState::ladder_Down_Left);
						return;
					}
				}
			};

		PlayerStates.CreateState(PlayerState::ladder_Down_Right, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Down_Stop_Left");
				//Capsule->MoveForce({ 0.0f,200.0f,0.0f }, Capsule->GetDir());
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Ladder_Check = false;
					Body_Col->On();
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);

					Capsule->GravityOn();
					Rabber_Collision_Check = false;
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}


			};

		PlayerStates.CreateState(PlayerState::ladder_Down_Stop_Left, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("ladder_Down_Stop_Right");
				//Capsule->MoveForce({ 0.0f,200.0f,0.0f }, Capsule->GetDir());
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Ladder_Check = false;
					Body_Col->On();
					Capsule->ResetMove(Enum_Axies::X | Enum_Axies::Z | Enum_Axies::Y);
					Capsule->GravityOn();
					Rabber_Collision_Check = false; 
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::ladder_Down_Stop_Right, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				StateValue = PlayerState::Death; 
				MainRenderer->ChangeAnimation("Death");	
				Body_Col->Off();
				Attack_Col->Off();
				Shield_Col->Off(); 
				Top_Shield_Col->Off();
				SetFlag(Enum_ActorFlag::Death, true);
				GameEnginePhysX::PushSkipCollisionPair(2, Enum_CollisionOrder::Player, Enum_CollisionOrder::Monster);
				SetFlag(Enum_ActorFlag::Death, true);
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					
					
					
				
						return; 
			
					

				}


			};

		PlayerStates.CreateState(PlayerState::Death, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Weak_Shield_block",true);
				StateValue = PlayerState::Shield_Idle;
				Stat.AddStamina(-60.0f);
				Attack_Col->Off(); 
				Shield_Col->On();
				Hit.SetHit(false);
				Top_Shield_Col->On();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				Hit.SetHit(false);

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd() && GameEngineInput::IsPress(VK_RBUTTON, this))
				{
					
					PlayerStates.ChangeState(PlayerState::Shield_Idle);
					return;
					
				}
				else if (MainRenderer->IsCurAnimationEnd())
				{
					Shield_Col->Off();
					Top_Shield_Col->Off();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}

				if (MainRenderer->GetCurAnimationFrame() > 16)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{


						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{


						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{


						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{


						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}
				}
			};
		PlayerStates.CreateState(PlayerState::Weak_Shield_block, NewPara);
	}


	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Middle_Shield_block");
				Hit.SetHit(false);
				StateValue = PlayerState::Shield_Idle;
				Stat.AddStamina(-105.0f);
				Attack_Col->Off();
				Shield_Col->On();
				Top_Shield_Col->On ();
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				Hit.SetHit(false);
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				
				if (MainRenderer->IsCurAnimationEnd() && GameEngineInput::IsPress(VK_RBUTTON, this))
				{
					
						PlayerStates.ChangeState(PlayerState::Shield_Idle);
						return;
					
				}
				else if (MainRenderer->IsCurAnimationEnd())
				{
					Shield_Col->Off();
					Top_Shield_Col->Off();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
				
				if (MainRenderer->GetCurAnimationFrame() > 36)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{


						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{


						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{


						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{


						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}
				}

			};
		PlayerStates.CreateState(PlayerState::Middle_Shield_block, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Shield_Col->Off();
				Top_Shield_Col->Off();
				MainRenderer->ChangeAnimation("Big_Shield_block");
				Hit.SetHit(false);
				StateValue = PlayerState::Shield_Idle;
				Attack_Col->Off();
				Stat.SetStamina(0.0f); 
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				Hit.SetHit(false);
				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}
				
				if (MainRenderer->IsCurAnimationEnd())
				{
					Shield_Col->Off();
					Top_Shield_Col->Off();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				};	

				if (MainRenderer->GetCurAnimationFrame() > 50)
				{
					if (true == GameEngineInput::IsPress('W', this))
					{
						MainRenderer->ChangeAnimation("Walk_Forward");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('S', this))
					{


						MainRenderer->ChangeAnimation("Walk_Behind");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('A', this))
					{


						MainRenderer->ChangeAnimation("Walk_Left");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}

					if (true == GameEngineInput::IsPress('D', this))
					{


						MainRenderer->ChangeAnimation("Walk_Right");
						PlayerStates.ChangeState(PlayerState::Move);
						return;
					}
				}

			};

		PlayerStates.CreateState(PlayerState::Big_Shield_block, NewPara);
	}

	{
		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				MainRenderer->ChangeAnimation("Parry_Attack");
				StateValue = PlayerState::Parring_Attack;
			};


		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{

				if (Rock_On_Check == true)
				{
					Capsule->SetWorldRotation({ 0.0f,degree_X });
					Actor_test->Transform.SetLocalRotation({ Rock_on_X,degree_X });
				}

				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Parring_Attack, NewPara);
	}

	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				Hit.SetHit(false);
				MainRenderer->ChangeAnimation("Attack_Block");
				Attack_Col->Off();
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					SetFlag(Enum_ActorFlag::Block_Shield, false);
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Attack_Block, NewPara);


	}


	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				StateValue = PlayerState::Sit_Down;
				MainRenderer->ChangeAnimation("Sit_Down");

				MainRenderer->ChangeCurFrame(60);
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					if (GameEngineInput::IsDown('W',this))
					{
						PlayerStates.ChangeState(PlayerState::Stand_Up);
						return;
					}
				}
				
			};

		PlayerStates.CreateState(PlayerState::Sit_Down, NewPara);


	}

	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Stand_Up");
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Stand_Up, NewPara);


	}


	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Stand_Up");
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Stand_Up, NewPara);


	}

	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Fail");
				testaaa = false;
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (testaaa == true)
				{
					PlayerStates.ChangeState(PlayerState::landing);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::fail, NewPara);
	}

	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("landing");
				testaaa = false;
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::landing, NewPara);
	}


	
	{

		CreateStateParameter NewPara;

		NewPara.Start = [=](class GameEngineState* _Parent)
			{
				MainRenderer->ChangeAnimation("Fire");
				Weapon_Actor->Getweapon()->Off();
			};

		NewPara.Stay = [=](float _DeltaTime, class GameEngineState* _Parent)
			{
				if (MainRenderer->IsCurAnimationEnd())
				{
					Weapon_Actor->Getweapon()->On();
					PlayerStates.ChangeState(PlayerState::Idle);
					return;
				}
			};

		PlayerStates.CreateState(PlayerState::Fire, NewPara);
	}




	PlayerStates.ChangeState(PlayerState::Idle);



}

void Player::SoundFrameEvent()
{
	// walk 
	MainRenderer->SetFrameEvent("Walk_Forward", 7, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Run", 8, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Run", 17, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Walk_Forward", 18, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Walk_Forward", 15, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-armor-2.wav", BoneWorldPos(0), 0.1f);		
		});

	MainRenderer->SetFrameEvent("Walk_Behind", 7, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});
	MainRenderer->SetFrameEvent("Walk_Behind", 18, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Walk_Behind", 15, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-armor-2.wav", BoneWorldPos(0), 0.1f);
		});

	MainRenderer->SetFrameEvent("Walk_Right", 7, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});
	MainRenderer->SetFrameEvent("Walk_Right", 18, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Walk_Right", 15, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-armor-2.wav", BoneWorldPos(0), 0.1f);
		});


	MainRenderer->SetFrameEvent("Walk_Left", 7, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});
	MainRenderer->SetFrameEvent("Walk_Left", 18, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0));
		});

	MainRenderer->SetFrameEvent("Walk_Left", 15, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-armor-2.wav", BoneWorldPos(0), 0.1f);
		});

	// roll

	MainRenderer->SetFrameEvent("Roll_Behind", 0, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-hauberk-rolling.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Roll_Right", 0, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-hauberk-rolling.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Roll_Left", 0, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-hauberk-rolling.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Roll_Forward", 0, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-hauberk-rolling.wav", BoneWorldPos(0), 0.3f);
		});

	MainRenderer->SetFrameEvent("Back_Step", 0, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("body-hauberk-rolling.wav", BoneWorldPos(0), 0.3f);
		});


	// Attack

	MainRenderer->SetFrameEvent("Attack_02", 13, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("swing-sword.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Attack_01", 13, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("swing-sword2.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Attack_03", 13, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("swing-sword-large.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Attack_04", 13, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("swing-sword2.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Attack_05", 13, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("swing-sword-large.wav", BoneWorldPos(0), 0.3f);
		});
	
	// shield

	MainRenderer->SetFrameEvent("Shield_Idle", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("movement2.wav", BoneWorldPos(0), 0.3f);
		});
	MainRenderer->SetFrameEvent("Parrying", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("c000004950.wav", BoneWorldPos(0), 0.3f);
		});



	// moveStop


	MainRenderer->SetFrameEvent("Left_Stop", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0), 0.7f);
		});
	MainRenderer->SetFrameEvent("Behind_Stop", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0), 0.7f);
		});
	MainRenderer->SetFrameEvent("Right_Stop", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0), 0.7f);
		});
	MainRenderer->SetFrameEvent("Forward_Stop", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-wa2.wav", BoneWorldPos(0), 0.7f);
		});


	// drink 
	MainRenderer->SetFrameEvent("Portion_Drink_01", 20, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("EST-drink.wav", BoneWorldPos(0), 1.0f);
		});

	// hit

	MainRenderer->SetFrameEvent("Hit_right", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Hit_Left", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Hit_Forward", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Hit_Behind", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Middle_Hit_Left", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Middle_Hit_Right", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Middle_Hit_Forward", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Middle_Hit_Behind", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("String_Hit_Forward", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});
	MainRenderer->SetFrameEvent("String_Hit_Behind", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Hit_Down", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("p_damage2.wav", BoneWorldPos(0), 0.8f);
		});


	MainRenderer->SetFrameEvent("Weak_Shield_block", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("shield-iron2.wav", BoneWorldPos(0), 0.8f);
		});

	MainRenderer->SetFrameEvent("Middle_Shield_block", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("shield-iron2.wav", BoneWorldPos(0), 0.8f);
		});
	MainRenderer->SetFrameEvent("Big_Shield_block", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("shield-iron2.wav", BoneWorldPos(0), 0.8f);
		});
	MainRenderer->SetFrameEvent("Attack_Block", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("shield-iron2.wav", BoneWorldPos(0), 0.8f);
		});

	// down 

	MainRenderer->SetFrameEvent("landing", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("foot-stone-l.wav", BoneWorldPos(0));
		});


	// 안개 
	MainRenderer->SetFrameEvent("Slow_Walk_Forward", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("Ladder.wav", BoneWorldPos(0), 0.8f);
		});

	// 사다리 

	MainRenderer->SetFrameEvent("ladder_Down_Left", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("Ladder.wav", BoneWorldPos(0), 0.7f);
		});
	MainRenderer->SetFrameEvent("ladder_Down_Right", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("Ladder.wav", BoneWorldPos(0), 0.7f);
		});
	MainRenderer->SetFrameEvent("ladder_Up_Right", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("Ladder.wav", BoneWorldPos(0), 0.7f);
		});
	MainRenderer->SetFrameEvent("ladder_Up_Left", 1, [&](GameContentsFBXRenderer* _Renderer)
		{
			GameEngineSound::Sound3DPlay("Ladder.wav", BoneWorldPos(0), 0.7f);
		});





}

float4 Player::BoneWorldPos(int _BoneIndex)
{
	std::vector<float4x4>& BoneMats = MainRenderer->GetBoneSockets();
	float4x4 BoneMatrix = BoneMats.at(_BoneIndex);

	float4x4 BoneWMat = BoneMatrix * Transform.GetWorldMatrix();
	float4 S;
	float4 R;
	float4 P;
	BoneWMat.Decompose(S, R, P);

	return P;
}