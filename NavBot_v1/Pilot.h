// Copyright (c) 2014, Solder Spot
// All rights reserved.
// See LICENSE.txt in root folder 

#ifndef __PILOT_H
#define __PILOT_H

#include "Navigator.h"
#include <stddef.h>

//----------------------------------------
//
//----------------------------------------

class Pilot
{
    public:

        enum State
        {
            PLT_STATE_STOPPED,
            PLT_STATE_TURNING,
            PLT_STATE_MOVING,
            PLT_STATE_STOPPING,
            PLT_NUM_STATES
        };

        enum Task
        {
            PLT_TASK_NONE,
            PLT_TASK_DONE,
            PLT_TASK_TURN,
            PLT_TASK_MOVE,
            PLT_TASK_STOP,
            PLT_NUM_TASKS
        };


        Pilot();

        typedef nvTime      TimeFunction( void );
        typedef bool        TicksHandler( Pilot *pilot, int16_t *lticks, int16_t *rticks);
        typedef void        MotorHandler( Pilot *pilot, int16_t lmotor, int16_t rmotor);

        // setters and getters
        void                SetNavigator( Navigator &nav ) { m_nav = &nav; }
        Navigator           *GetNavigator( void ) { return m_nav; }
        void                SetTimeFunction( TimeFunction *func ) { m_time_func = func; }
        void                SetTicksHandler( TicksHandler *handler, void *data = NULL ) { m_ticks_handler = handler; m_ticks_handler_data = data; }
        void                SetMotorHandler( MotorHandler *handler, void *data = NULL ) { m_motor_handler = handler; m_motor_handler_data = data; }
        void                *TicksData( void ) { return m_ticks_handler_data; }
        void                *MotorData( void ) { return m_motor_handler_data; }
        void                SetMaxMoveSpeed( nvRate mm_per_second ) { m_max_move_speed = mm_per_second; }
        void                SetMaxTurnSpeed( nvRate degrees_per_second ) { m_max_turn_speed = degrees_per_second; }
        void                SetMinUpdateInterval( nvTime interval ) { m_min_update_interval = interval; m_hPID.minDelta = m_sPID.minDelta = (float)interval; }
        nvTime              MinUpdateInterval( void ) { return m_min_update_interval; }
        void                SetHeadingPID( float Kp, float Ki, float Kd ) { m_hPID.SetKs( Kp, Ki, Kd); }
        void                SetSpeedPID( float Kp, float Ki, float Kd ) { m_sPID.SetKs( Kp, Ki, Kd); }

        // methods          
        void                Reset( void );
        void                Service( void );
        void                Stop( void );
        void                Move( nvDistance distance );
        void                Turn( nvHeading degrees );
        bool                IsDone( void ) { return m_task == PLT_TASK_DONE; }

    protected:

        // external references
        Navigator           *m_nav;
        TimeFunction        *m_time_func;
        TicksHandler        *m_ticks_handler;
        void                *m_ticks_handler_data;
        MotorHandler        *m_motor_handler;
        void                *m_motor_handler_data;

        // config settings
        nvRate              m_max_move_speed;
        nvRate              m_max_turn_speed;
        nvTime              m_min_update_interval;

        // logic states
        Task                m_task;
        State               m_state;
        nvPosition          m_target_pos;
        nvHeading           m_target_heading;
        nvDistance          m_target_dist;
        nvTime              m_last_time;
		nvTime				m_dt;
		bool				m_end_task_on_stop;

        // motor control
        int16_t             m_mpower;
		int16_t             m_ladjust;
		int16_t             m_radjust;
        int16_t             m_ldir;
        int16_t             m_rdir;
		int16_t				m_mp_start;			// power level that motors will start movement 
		int16_t 			m_mp_stop;			// power level at which motors will stop movement
		bool				m_was_in_motion;	// detect start and stop of movement

		void				update_motors( void );
		void				adjust_mpower( int16_t );
        void                full_stop( void );
        void                update_turn( void );
        void                update_move( void );


		// PID
		struct PIDController
		{
			PIDController();

			float 		Kp;
			float 		Ki;
			float 		Kd;
			float		sumErrs;
			float		lastErr;
			float		minDelta;
			float		target;
			float		CalcAdjustment( float input, nvTime dt );
			void		Reset( void );
			void		SetTarget( float t ) { target = t;}
			void 		SetKs( float p, float i, float d ) { Kp = p; Ki = i; Kd = d; }
		};

		PIDController		m_hPID;			// PID for heading control
		PIDController		m_sPID;			// PID for speed control
		PIDController		m_tPID;			// PID for turn control


		nvTime 				getTime( void ) { return m_time_func(); }
};

#endif __PILOT_H
