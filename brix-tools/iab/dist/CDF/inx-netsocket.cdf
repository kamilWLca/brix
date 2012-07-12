﻿<?xml version="1.0" encoding="UTF-8"?>
<Component>
  <Icon>inx-netsocket.bmp</Icon>
  <Class>netSocket</Class>
  <Module>usercomponents</Module>
  <Menu>Networking<Menu>socket client</Menu></Menu>
  <ShortDescription>network Socket Component</ShortDescription>
  <LongDescription>provides tcpip and udp socket data comms.</LongDescription>
  <Parameter>
    <Name>URL</Name>
    <DataType>S</DataType>
    <DefaultValue/>
    <MinValue>0</MinValue>
    <MaxValue>256</MaxValue>
    <Description>URL of the server socket. N/A for sockets</Description>
    <ListPlacement>1</ListPlacement>
    <ArgPlacement>1</ArgPlacement>
  </Parameter>
  <Parameter>
    <Name>port</Name>
    <DataType>I</DataType>
    <DefaultValue>8000</DefaultValue>
    <MinValue>0</MinValue>
    <MaxValue>65535</MaxValue>
    <Description>IP socket number to open</Description>
    <ListPlacement>2</ListPlacement>
    <ArgPlacement>2</ArgPlacement>
  </Parameter>
  <Parameter>
    <Name>tcp/udp</Name>
    <DataType>I</DataType>
    <DefaultValue>0</DefaultValue>
    <MinValue>0</MinValue>
    <MaxValue>1</MaxValue>
    <Description>0=tcp, 1=udp</Description>
    <ListPlacement>3</ListPlacement>
    <ArgPlacement>3</ArgPlacement>
  </Parameter>
  <Port>
    <Description>open</Description>
    <PortType>StartPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>10</YCoordinate>
    <AtomicFlag>1</AtomicFlag>
    <FunctionName argument="0">open</FunctionName>
  </Port>
  <Port>
    <Description>close</Description>
    <PortType>StartPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>90</YCoordinate>
    <AtomicFlag>1</AtomicFlag>
    <FunctionName argument="0">close</FunctionName>
  </Port>
  <Port>
    <DataType>I</DataType>
    <Description>port</Description>
    <PortType>InputPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>50</YCoordinate>
    <FunctionName argument="1">open</FunctionName>
  </Port>
  <Port>
    <Description>send</Description>
    <PortType>StartPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>190</YCoordinate>
    <AtomicFlag>1</AtomicFlag>
    <FunctionName argument="0">send</FunctionName>
  </Port>
  <Port>
    <Description>error</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>110</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="1">close</FunctionName>
    <FunctionName argument="7">open</FunctionName>
  </Port>
  <Port>
    <Description>error</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>230</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="1">send</FunctionName>
    <FunctionName argument="6">open</FunctionName>
  </Port>
  <Port>
    <Description>----</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>20</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="1">open</FunctionName>
  </Port>
  <Port>
    <Description>error</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>40</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="2">open</FunctionName>
  </Port>
  <Port>
    <DataType>S</DataType>
    <Description>url</Description>
    <PortType>InputPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>30</YCoordinate>
    <FunctionName argument="2">open</FunctionName>
  </Port>
  <Port>
    <DataType>S</DataType>
    <Description>data</Description>
    <PortType>InputPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>210</YCoordinate>
    <FunctionName argument="1">send</FunctionName>
  </Port>
  <Port>
    <DataType>I</DataType>
    <Description>errno</Description>
    <PortType>OutputPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>250</YCoordinate>
    <FunctionName argument="1">open</FunctionName>
    <FunctionName argument="1">close</FunctionName>
    <FunctionName argument="1">send</FunctionName>
  </Port>
  <Port>
    <DataType>S</DataType>
    <Description>data</Description>
    <PortType>OutputPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>160</YCoordinate>
    <FunctionName argument="2">open</FunctionName>
  </Port>
  <Port>
    <Description>received</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>140</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="3">open</FunctionName>
  </Port>
  <Port>
    <DataType>I</DataType>
    <Description>size</Description>
    <PortType>InputPort</PortType>
    <XCoordinate>0</XCoordinate>
    <YCoordinate>230</YCoordinate>
    <FunctionName argument="2">send</FunctionName>
  </Port>
  <Port>
    <DataType>I</DataType>
    <Description>size</Description>
    <PortType>OutputPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>180</YCoordinate>
    <FunctionName argument="3">open</FunctionName>
  </Port>
  <Port>
    <Description>---</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>90</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="4">open</FunctionName>
    <FunctionName argument="2">close</FunctionName>
  </Port>
  <Port>
    <Description>---</Description>
    <PortType>FinishPort</PortType>
    <XCoordinate>165</XCoordinate>
    <YCoordinate>210</YCoordinate>
    <Wcet>0</Wcet>
    <FunctionName argument="2">send</FunctionName>
    <FunctionName argument="5">open</FunctionName>
  </Port>
</Component>
