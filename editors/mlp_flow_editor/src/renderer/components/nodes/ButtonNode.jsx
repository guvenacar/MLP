import React, { memo } from 'react';
import { Handle, Position, NodeResizer } from 'reactflow';

const ButtonNode = ({ data, selected }) => {
  const width = data.width || 100;
  const height = data.height || 32;
  
  const defaultStyle = {
    backgroundColor: '#3c3c3c',
    color: '#cccccc',
    border: '1px solid #555',
    borderRadius: '3px',
    fontSize: '13px',
    fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif',
    cursor: 'pointer',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    userSelect: 'none',
  };

  return (
    <div
      style={{
        ...defaultStyle,
        ...data.style,
        width: `${width}px`,
        height: `${height}px`,
        border: selected ? '2px solid #007acc' : '1px solid #555',
      }}
    >
      <NodeResizer
        color="#007acc"
        isVisible={selected}
        minWidth={60}
        minHeight={24}
      />
      {data.label || 'Button'}
      <Handle type="target" position={Position.Top} style={{ opacity: 0 }} />
      <Handle type="source" position={Position.Bottom} style={{ opacity: 0 }} />
    </div>
  );
};

export default memo(ButtonNode);
