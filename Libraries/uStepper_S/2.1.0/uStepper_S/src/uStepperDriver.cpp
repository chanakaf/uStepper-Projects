/********************************************************************************************
* 	 	File: 		uStepperDriver.cpp														*
*		Version:    2.1.0                                           						*
*      	Date: 		July 11th, 2020  	                                    				*
*      	Author: 	Thomas Hørring Olsen                                   					*
*                                                   										*	
*********************************************************************************************
*	(C) 2020																				*
*																							*
*	uStepper ApS																			*
*	www.ustepper.com 																		*
*	administration@ustepper.com 															*
*																							*
*	The code contained in this file is released under the following open source license:	*
*																							*
*			Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International			*
* 																							*
* 	The code in this file is provided without warranty of any kind - use at own risk!		*
* 	neither uStepper ApS nor the author, can be held responsible for any damage				*
* 	caused by the use of the code contained in this file ! 									*
*                                                                                           *
* TEST *
********************************************************************************************/
/**
* @file uStepperDriver.cpp
*
* @brief      Function implementations for the TMC5130 motor driver
*
*             This file contains class and function implementations for the TMC5130 motor driver.
*
* @author     Thomas Hørring Olsen (thomas@ustepper.com)
*/
#include <uStepperS.h>

extern uStepperS * pointer;

uStepperDriver::uStepperDriver( void ){
}


void uStepperDriver::reset( void ){

	// Reset stallguard
	this->writeRegister( TCOOLTHRS, 0 );
	this->writeRegister( THIGH, 	0);
	this->writeRegister( COOLCONF, 	0 );
	this->writeRegister( SW_MODE, 	0 );
	this->clearStall();

	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);
	
	this->writeRegister( IHOLD_IRUN,0 );
	this->writeRegister( CHOPCONF, 	0 );
	this->writeRegister( GCONF, 	0 );
	this->writeRegister( PWMCONF, 	0 );	
	this->writeRegister( TPWMTHRS, 	0 );

	this->writeRegister( RAMPMODE, 	0 );
	this->writeRegister( VSTART, 	0 );
	this->writeRegister( A1, 		0 ); 
	this->writeRegister( V1, 		0 ); 
	this->writeRegister( AMAX, 		0 );
	this->writeRegister( VMAX, 		0 );
	this->writeRegister( D1, 		0 );
	this->writeRegister( VSTOP, 	0 );

}

void uStepperDriver::init( uStepperS * _pointer ){

	this->pointer = _pointer;
	this->chipSelect(true); // Set CS HIGH

	// First clear previous defined registers
	this->reset();

	/* Set motor current */
	this->writeRegister( IHOLD_IRUN, IHOLD( this->holdCurrent) | IRUN( this->current) | IHOLDDELAY( this->holdDelay) );

	this->enableStealth();

	/* Set all-round chopper configuration */
	this->writeRegister( CHOPCONF, TOFF(2) | TBL(2) | HSTRT_TFD(4) | HEND(0));

	/* Set startup ramp mode */
	this->setRampMode( POSITIONING_MODE );

	/* Reset position */
	this->writeRegister(XACTUAL, 0);
	this->writeRegister(XTARGET, 0);
	
	this->setDeceleration( 0xFFFE );
	this->setAcceleration( 0xFFFE );
	
	this->stop();

	while(this->readRegister(VACTUAL) != 0);
}

void uStepperDriver::readMotorStatus(void)
{
	while(TCNT1 > 15900);		//If interrupt is just about to happen, wait for it to finish
	this->readRegister(XACTUAL);
}

void uStepperDriver::setVelocity( uint32_t velocity )
{
	this->VMAX = velocity;

	if(this->VMAX > 0x7FFE00)
	{
		this->VMAX = 0x7FFE00;
	}
	
	this->writeRegister(VMAX_REG, this->VMAX);
}

void uStepperDriver::setAcceleration( uint32_t acceleration )
{
	this->AMAX = acceleration;

	if(this->AMAX > 0xFFFE)
	{
		this->AMAX = 0xFFFE;
	}

	this->writeRegister(AMAX_REG, this->AMAX);
}

void uStepperDriver::setDeceleration( uint32_t deceleration )
{
	this->DMAX = deceleration;

	if(this->DMAX > 0xFFFE)
	{
		this->DMAX = 0xFFFE;
	}

	this->writeRegister(DMAX_REG, this->DMAX);
}

void uStepperDriver::setCurrent( uint8_t current )
{
	this->current = current;
	this->updateCurrent();
}

void uStepperDriver::setHoldCurrent( uint8_t current )
{
	this->holdCurrent = current;
	this->updateCurrent();
}

void uStepperDriver::updateCurrent( void )
{
	this->writeRegister( IHOLD_IRUN, IHOLD( this->holdCurrent) | IRUN( this->current) | IHOLDDELAY( this->holdDelay) );
}

void uStepperDriver::setPosition( int32_t position )
{
	this->mode = DRIVER_POSITION;
	this->setRampMode(POSITIONING_MODE);
	this->writeRegister(XTARGET, position);
	this->xTarget = position;
}

void uStepperDriver::setShaftDirection( bool direction )
{
	// Read the register to save the settings
	int32_t value = this->readRegister( GCONF );
	// Update the direction bit
	if(direction == 1){
		value |= (0x01 << 4);
	}else{
		value &= ~(0x01 << 4);
	}
	this->writeRegister( GCONF, value ); 
}

void uStepperDriver::setDirection( bool direction )
{
	this->mode = DRIVER_VELOCITY;
	if(direction){
		this->writeRegister( RAMPMODE, VELOCITY_MODE_POS ); 
	}else{
		this->writeRegister( RAMPMODE, VELOCITY_MODE_NEG ); 
	}
}

void uStepperDriver::setRampMode( uint8_t mode ){

	switch(mode){
		case POSITIONING_MODE:
			// Positioning mode
			this->writeRegister(VSTART_REG, this->VSTART);
			this->writeRegister(A1_REG, 	this->A1); 					
			this->writeRegister(V1_REG, 	this->V1); 					
			this->writeRegister(AMAX_REG, 	this->AMAX); 	
			this->writeRegister(VMAX_REG, 	this->VMAX);
			this->writeRegister(DMAX_REG, 	this->DMAX); 	
			this->writeRegister(D1_REG, 	this->D1); 					
			this->writeRegister(VSTOP_REG, 	this->VSTOP); /* Minimum 10 in POSITIONING_MODE */				
			this->writeRegister(RAMPMODE, POSITIONING_MODE); /* RAMPMODE = POSITIONING_MODE */
		break;

		case VELOCITY_MODE_POS:
			// Velocity mode (only AMAX and VMAX is used)
			this->writeRegister(VSTART_REG, this->VSTART);
			this->writeRegister(A1_REG, 	0); 						
			this->writeRegister(V1_REG, 	0); 
			this->writeRegister(AMAX_REG, 	this->AMAX); 	
			this->writeRegister(VMAX_REG, 	this->VMAX);
			this->writeRegister(DMAX_REG, 	0); 
			this->writeRegister(D1_REG, 	0); 					
			this->writeRegister(VSTOP_REG, 	0); 		
			this->writeRegister(RAMPMODE, VELOCITY_MODE_POS); /* RAMPMODE = VELOCITY_MODE_POS */
		break;
	}
}

void uStepperDriver::enableStealth()
{
	/* Set GCONF and enable stealthChop */
	this->writeRegister( GCONF, EN_PWM_MODE(1) | I_SCALE_ANALOG(1) ); 
	this->setShaftDirection(pointer->shaftDir);

	/* Set PWMCONF for StealthChop */
	this->writeRegister( PWMCONF, PWM_AUTOSCALE(1) | PWM_GRAD(1) | PWM_AMPL(128) | PWM_FREQ(0) | FREEWHEEL(2) ); 

	/* Specifies the upper velocity (lower time delay) for operation in stealthChop voltage PWM mode */
	this->writeRegister( TPWMTHRS, 5000 ); 
}

int32_t uStepperDriver::getVelocity( void )
{
	return this->readRegister(VACTUAL);
}

int32_t uStepperDriver::getPosition( void )
{
	return this->readRegister(XACTUAL);
}

void uStepperDriver::stop( void )
{
	this->mode = DRIVER_STOP;
	this->setVelocity(0);
}

void uStepperDriver::setHome(int32_t initialSteps)
{
	int32_t xActual, xTarget;

	if(this->mode == DRIVER_POSITION)
	{
		xActual = this->getPosition();
		xTarget = this->readRegister(XTARGET);

		xTarget -= xActual;
		this->xTarget = xTarget + initialSteps;
		this->xActual = initialSteps;
		this->writeRegister(XACTUAL, initialSteps);
		this->writeRegister(XTARGET, this->xTarget);
	}
	else
	{
		this->xTarget = initialSteps;
		this->xActual = initialSteps;
		this->writeRegister(XACTUAL, initialSteps);
		this->writeRegister(XTARGET, initialSteps);
	}

	pointer->pidPositionStepsIssued = initialSteps;
}

int32_t uStepperDriver::writeRegister( uint8_t address, uint32_t datagram ){

	// Disabled interrupts until write is complete
	//cli();
	TIMSK1 &= ~(1 << OCIE1A);
	// Enable SPI mode 3 to use TMC5130
	this->pointer->setSPIMode(3);

	uint32_t package = 0;

	// Add the value of WRITE_ACCESS to enable register write
	address += WRITE_ACCESS;

	this->chipSelect(false);

	this->status = this->pointer->SPI(address);

	package |= this->pointer->SPI((datagram >> 24) & 0xff);
	package <<= 8;
	package |= this->pointer->SPI((datagram >> 16) & 0xff);
	package <<= 8;
	package |= this->pointer->SPI((datagram >> 8) & 0xff);
	package <<= 8;
	package |= this->pointer->SPI((datagram) & 0xff);

	this->chipSelect(true); // Set CS HIGH

	//sei(); 
	TIMSK1 |= (1 << OCIE1A);
	return package;
}

int32_t uStepperDriver::readRegister( uint8_t address )
{
	// Disabled interrupts until write is complete
	//cli();
	TIMSK1 &= ~(1 << OCIE1A);

	// Enable SPI mode 3 to use TMC5130
	this->pointer->setSPIMode(3);

	// Request a reading on address
	this->chipSelect(false);
	this->status = this->pointer->SPI(address);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->pointer->SPI(0x00);
	this->chipSelect(true);

	// Read the actual value on second request
	int32_t value = 0;

	this->chipSelect(false);
	this->status = this->pointer->SPI(address);
	value |= this->pointer->SPI(0x00);
	value <<= 8;
	value |= this->pointer->SPI(0x00);
	value <<= 8;
	value |= this->pointer->SPI(0x00);
	value <<= 8;
	value |= this->pointer->SPI(0x00);
	this->chipSelect(true);

	//sei(); 
	TIMSK1 |= (1 << OCIE1A);

	return value;
}

void uStepperDriver::chipSelect(bool state)
{
	if(state == false)
		PORTE &= ~(1 << CS_DRIVER);  // Set CS LOW 
	else
		PORTE |= (1 << CS_DRIVER); // Set CS HIGH
}

void uStepperDriver::enableStallguard( int8_t threshold, bool stopOnStall, float rpm)
{
	// Limit threshold
	if( threshold > 63)
		threshold = 63;
	else if( threshold < -64)
		threshold = -64;

	rpm = abs(rpm);
	// Limit rpm
	if( rpm > 1000)
		rpm = 1000;
	else if( rpm < 2)
		rpm = 2;

	/* Disable StealthChop for stallguard operation */
	this->writeRegister( GCONF, EN_PWM_MODE(0) | I_SCALE_ANALOG(1) ); 
	this->setShaftDirection(pointer->shaftDir);

	// Configure COOLCONF for stallguard
	this->writeRegister( COOLCONF, SGT(threshold) | SFILT(1) | SEMIN(5) | SEMAX(2) | SEDN(1) );

	//int32_t stall_speed = 1048576 / pointer->rpmToVelocity * speed // 1048576 = 2^20. See TSTEP in datasheet p.33
	int32_t stall_speed = 1048576 / pointer->rpmToVelocity * (rpm/2); //Should be 1048576 = 2^20.
	stall_speed = stall_speed * 1.2; // // Activate stallGuard sligthly below desired homing velocity (provide 20% tolerance)

	// Set TCOOLTHRS to max speed value (enable stallguard for all speeds)
	this->writeRegister( TCOOLTHRS, stall_speed ); // Max value is 20bit = 0xFFFFF
	this->writeRegister( THIGH, 0);
	
	// Enable automatic stop on stall dectection
	if( stopOnStall )
		this->writeRegister( SW_MODE, SG_STOP(1) );
	else
		this->writeRegister( SW_MODE, SG_STOP(0) );
}

void uStepperDriver::disableStallguard( void )
{
	// Reenable stealthchop
	this->writeRegister( GCONF, EN_PWM_MODE(1) | I_SCALE_ANALOG(1) );
	this->setShaftDirection(pointer->shaftDir);

	// Disable all stallguard configuration
	this->writeRegister( COOLCONF, 	0 );
	this->writeRegister( TCOOLTHRS, 0 );
	this->writeRegister( THIGH, 	0);
	this->writeRegister( SW_MODE, 	0 );	
}

void uStepperDriver::clearStall( void )
{
	// Reading the RAMP_STAT register clears the stallguard flag, telling the driver to continue. 
	this->readRegister( RAMP_STAT );
}

uint16_t uStepperDriver::getStallValue( void )
{
	// Get the SG_RESULT from DRV_STATUS. 
	return this->readRegister(DRV_STATUS) & 0x3FF;
}
