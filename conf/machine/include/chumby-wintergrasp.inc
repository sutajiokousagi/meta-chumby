#@TYPE: Machine
#@Name: Silvermoon Armada 100 Development Platform
#@DESCRIPTION: Machine configuration for chumby Silvermoon board

CNPLATFORM = "${@bb.data.getVar('MACHINE', d, 1).replace('chumby-', '')}"

