# arduino-OrientalAZ (OrientalAZ_asukiaaa)

A library for arduino to use step motor driver AZ series by Orientalmotor.

## Role of switches

### Baud

Number | Baudrate
0 | 9600
1 | 19200
2 | 38400
3 | 57600
4 | 115200
5 | 230400
6 | Not used
7 | Network converter
8-f | Not used

### SW1

Number | Role
1 | Address config. Plus 16 when on.
2 | Protocol select: on is modbus, off is network converter
3 | RS485 end resistor
4 | RS485 end resistor

### ID

Address select for RS485 communication.

0 is used for broad cast.
If SW1-1 is on, address become plus 16.

## Connector

### CN7, CN8 (RJ45)

Number | Role
1 | Not used
2 | GND
3 | RS485 plus
4 | Not used
5 | Not used
6 | RS485 minus
7 | Not used
8 | Not used

## License

MIT

## References

- Manuals [ja](https://www.orientalmotor.co.jp/download/manual_search.action?productName=AZD-KD&searchPattern=1&gengoId=1), [en](https://www.orientalmotor.co.jp/download/manual_search.action?productName=AZD-KD&searchPattern=1&gengoId=2&x=51&y=29)
[HM-60252J](https://www.orientalmotor.co.jp/download/2dialog.action?serviceTypeNo=18&senimotoId=1&senimotoUrl=%2fproducts%2fdetail.action%3fhinmei%3dAZD-KD&hinmei=AZD-KD&gengo=JP&documentType=5&fileName=%2ffile_addon_auth%2fproducts%2fst%2fmanual%2fHM-60252J.pdf&gengoId=1) or [HM-60262E](https://www.orientalmotor.co.jp/download/2dialog.action?serviceTypeNo=18&senimotoId=1&senimotoUrl=%2fproducts%2fdetail.action%3fhinmei%3dAZD-KD&hinmei=AZD-KD&gengo=JP&documentType=5&fileName=%2ffile_addon_auth%2fproducts%2fst%2fmanual%2fHM-60262E.pdf&gengoId=2) has information about modbus communication.
