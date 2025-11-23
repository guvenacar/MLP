// Unified domain model (initial simple version)
// Widget schema: { id, type, geom:{x,y,width,height}, style:{fill, stroke}, data:{label} }

export function createWidget(id, x, y, width, height, fill, label = id) {
  return {
    id,
    type: 'box',
    geom: { x, y, width, height },
    style: { fill, stroke: 0x222222 },
    data: { label },
    state: { selected: false }
  };
}

export function createSampleWidgets(count = 100) {
  const arr = [];
  for (let i = 0; i < count; i++) {
    const fill = (50 + (i * 13) % 200) << 16 | (80 + (i * 29) % 140) << 8 | (120 + (i * 17) % 100);
    const x = 20 + (i % 10) * 90;
    const y = 20 + Math.floor(i / 10) * 60;
    arr.push(createWidget(`box-${i + 1}`, x, y, 80, 50, fill));
  }
  return arr;
}
