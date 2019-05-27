GRAMMAR
-------
NSOM's grammar.

NSOM is formulated json that conform to grammar follows:

```
network = { name [, node, channel, subnet] }
node    = { name [, point, netifs, config] }
subnet  = { name [, point, netifs, config, load] }
channel = { name [, point, type, config] }
netifs  = { connect, up }
config  = { config-obj }
point   = { x, y [, z] }
up      = name

config-obj = JSON-OBJECT
connect    = STRING
load       = STRING
x          = NUMBER
y          = NUMBER
z          = NUMBER
name       = STRING
```
