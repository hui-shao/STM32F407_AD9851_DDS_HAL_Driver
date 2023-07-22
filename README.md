硬件连接:
AD985x			单片机
AD985x_rst 	        PD4
					AD985x_clk 	    PD0
					AD985x_fq 	    PD5
					D0-07 			PD0-PD7 // 串行模式下，只使用 D7 ，此次连接到单片机 PD3
					5V					--5V
					GND					--GND(0V)

注意：可以参考CubeMX