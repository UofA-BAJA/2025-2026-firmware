namespace WebApplication2.Models;

public class Temperature
{
    public int Id { get; set; }
    public int Epoch { get; set; }  
    public int Value { get; set; }
    public int BeltId { get; set; }
    public required Belt Belt { get; set; }
}
