import React, { useRef, useEffect } from 'react';
import { Group, Rect, Text, Transformer } from 'react-konva';

const KonvaWidget = ({ widget, isSelected, onSelect, onUpdate, onDoubleClick }) => {
  const shapeRef = useRef();
  const trRef = useRef();

  // Seçili widget için transformer ekle
  useEffect(() => {
    if (isSelected && trRef.current && shapeRef.current) {
      trRef.current.nodes([shapeRef.current]);
      trRef.current.getLayer().batchDraw();
    }
  }, [isSelected]);

  // Widget tipine göre render
  const renderWidget = () => {
    switch (widget.type) {
      case 'button':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              fill={widget.backgroundColor}
              cornerRadius={5}
              shadowColor="black"
              shadowBlur={5}
              shadowOpacity={0.3}
              shadowOffsetX={2}
              shadowOffsetY={2}
            />
            <Text
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              text={widget.label}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill={widget.textColor}
              align="center"
              verticalAlign="middle"
              padding={5}
            />
          </>
        );

      case 'label':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              fill="transparent"
            />
            <Text
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              text={widget.label}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill={widget.textColor}
              align="left"
              verticalAlign="middle"
              padding={2}
            />
          </>
        );

      case 'textbox':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              fill="#ffffff"
              stroke="#cccccc"
              strokeWidth={1}
              cornerRadius={3}
            />
            <Text
              x={5}
              y={0}
              width={widget.width - 10}
              height={widget.height}
              text={widget.label || ''}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill="#333333"
              verticalAlign="middle"
            />
          </>
        );

      case 'checkbox':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={20}
              height={20}
              fill="#ffffff"
              stroke="#007bff"
              strokeWidth={2}
              cornerRadius={3}
            />
            <Text
              x={25}
              y={0}
              width={widget.width - 25}
              height={20}
              text={widget.label}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill={widget.textColor}
              verticalAlign="middle"
            />
          </>
        );

      case 'radio':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={20}
              height={20}
              fill="#ffffff"
              stroke="#007bff"
              strokeWidth={2}
              cornerRadius={10}
            />
            <Text
              x={25}
              y={0}
              width={widget.width - 25}
              height={20}
              text={widget.label}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill={widget.textColor}
              verticalAlign="middle"
            />
          </>
        );

      case 'panel':
      case 'groupbox':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              fill={widget.backgroundColor || '#2d2d30'}
              stroke="#3e3e42"
              strokeWidth={1}
              cornerRadius={5}
            />
            <Text
              x={10}
              y={10}
              text={widget.label}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill={widget.textColor}
            />
          </>
        );

      case 'listbox':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              fill="#ffffff"
              stroke="#cccccc"
              strokeWidth={1}
              cornerRadius={3}
            />
            <Text
              x={5}
              y={5}
              text={widget.label || 'Liste'}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill="#333333"
            />
          </>
        );

      case 'combobox':
        return (
          <>
            <Rect
              x={0}
              y={0}
              width={widget.width}
              height={widget.height}
              fill="#ffffff"
              stroke="#cccccc"
              strokeWidth={1}
              cornerRadius={3}
            />
            <Text
              x={5}
              y={0}
              width={widget.width - 30}
              height={widget.height}
              text={widget.label || 'Seçiniz...'}
              fontSize={widget.fontSize}
              fontFamily={widget.fontFamily}
              fill="#333333"
              verticalAlign="middle"
            />
            <Text
              x={widget.width - 25}
              y={0}
              width={20}
              height={widget.height}
              text="▼"
              fontSize={10}
              fill="#666666"
              align="center"
              verticalAlign="middle"
            />
          </>
        );

      default:
        return (
          <Rect
            x={0}
            y={0}
            width={widget.width}
            height={widget.height}
            fill={widget.backgroundColor}
            stroke="#cccccc"
            strokeWidth={1}
          />
        );
    }
  };

  return (
    <>
      <Group
        ref={shapeRef}
        x={widget.x}
        y={widget.y}
        draggable
        onClick={onSelect}
        onTap={onSelect}
        onDblClick={() => {
          if (onDoubleClick) {
            onDoubleClick(widget.id);
          }
        }}
        onDblTap={() => {
          if (onDoubleClick) {
            onDoubleClick(widget.id);
          }
        }}
        onDragEnd={(e) => {
          onUpdate({
            x: e.target.x(),
            y: e.target.y()
          });
        }}
        onTransformEnd={() => {
          const node = shapeRef.current;
          const scaleX = node.scaleX();
          const scaleY = node.scaleY();

          node.scaleX(1);
          node.scaleY(1);

          onUpdate({
            x: node.x(),
            y: node.y(),
            width: Math.max(20, node.width() * scaleX),
            height: Math.max(20, node.height() * scaleY)
          });
        }}
      >
        {renderWidget()}
      </Group>

      {isSelected && (
        <Transformer
          ref={trRef}
          boundBoxFunc={(oldBox, newBox) => {
            if (newBox.width < 20 || newBox.height < 20) {
              return oldBox;
            }
            return newBox;
          }}
          enabledAnchors={[
            'top-left',
            'top-right',
            'bottom-left',
            'bottom-right',
            'middle-left',
            'middle-right'
          ]}
        />
      )}
    </>
  );
};

export default KonvaWidget;
