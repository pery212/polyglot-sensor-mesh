#[repr(C, packed)] //Use C style memory management
struct SensorPacket {
    node_id: u8,
    msg_id: u16,
    value: f32,
    timestamp: u32,
}